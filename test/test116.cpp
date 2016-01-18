#include "preprocessor_defines.dat"
#include "Class_Global.hpp"
#include "Class_Para_Tree.hpp"
#include "User_Data_Comm.hpp"
#include "User_Data_LB.hpp"

using namespace std;

// =================================================================================== //

int main(int argc, char *argv[]) {

#if NOMPI==0
	MPI::Init(argc, argv);

	{
#endif
		int iter = 0;

		/**<Instantation of a 3D para_tree object.*/
		Class_Para_Tree<3> pablo116;

		/**<Set NO 2:1 balance for the octree.*/
		int idx = 0;
		pablo116.setBalance(idx,false);

		/**<Refine globally five level and write the para_tree.*/
		for (iter=1; iter<6; iter++){
			pablo116.adaptGlobalRefine();
		}

#if NOMPI==0
		/**<PARALLEL TEST: Call loadBalance, the octree is now distributed over the processes.*/
		pablo116.loadBalance();
#endif

		/**<Define a center point and a radius.*/
		double xc, yc;
		xc = yc = 0.5;
		double radius = 0.25;

		/**<Define vectors of data.*/
		uint32_t nocts = pablo116.getNumOctants();
		uint32_t nghosts = pablo116.getNumGhosts();
		vector<double> oct_data(nocts, 0.0), ghost_data(nghosts, 0.0);

		/**<Assign a data (distance from center of a cylinder) to the octants with at least one node inside the cylinder.*/
		for (int i=0; i<nocts; i++){
			/**<Compute the nodes of the octant.*/
			vector<vector<double> > nodes = pablo116.getNodes(i);
			/**<Compute the center of the octant.*/
			vector<double> center = pablo116.getCenter(i);
			for (int j=0; j<global3D.nnodes; j++){
				double x = nodes[j][0];
				double y = nodes[j][1];
				if ((pow((x-xc),2.0)+pow((y-yc),2.0) <= pow(radius,2.0))){
					oct_data[i] = (pow((center[0]-xc),2.0)+pow((center[1]-yc),2.0));
				}
			}
		}

		/**<Update the connectivity and write the para_tree.*/
		iter = 0;
		pablo116.updateConnectivity();
		pablo116.writeTest("Pablo116_iter"+to_string(iter), oct_data);

		/**<Adapt two times with data injection on new octants.*/
		int start = 1;
		for (iter=start; iter<start+2; iter++){
			for (int i=0; i<nocts; i++){
				/**<Compute the nodes of the octant.*/
				vector<vector<double> > nodes = pablo116.getNodes(i);
				/**<Compute the center of the octant.*/
				vector<double> center = pablo116.getCenter(i);
				for (int j=0; j<global3D.nnodes; j++){
					double x = nodes[j][0];
					double y = nodes[j][1];
					if ((pow((x-xc),2.0)+pow((y-yc),2.0) <= pow(radius,2.0))){
						if (center[0]<=xc){

							/**<Set to refine to the octants in the left side of the domain inside a circle.*/
							pablo116.setMarker(i,1);
						}
						else{

							/**<Set to coarse to the octants in the right side of the domain inside a circle.*/
							pablo116.setMarker(i,-1);
						}
					}
				}
			}

			/**<Adapt the octree and map the data in the new octants.*/
			vector<double> oct_data_new;
			vector<uint32_t> mapper;
			pablo116.adapt(mapper);
			nocts = pablo116.getNumOctants();
			oct_data_new.resize(nocts, 0.0);

			/**<Assign to the new octant the average of the old children if it is new after a coarsening;
			 * while assign to the new octant the data of the old father if it is new after a refinement.
			 */
			for (int i=0; i<nocts; i++){
				if (pablo116.getIsNewC(i)){
					for (int j=0; j<global3D.nchildren; j++){
						oct_data_new[i] += oct_data[mapper[i]+j]/global3D.nchildren;
					}
				}
				else if (pablo116.getIsNewR(i)){
					oct_data_new[i] += oct_data[mapper[i]];
				}
				else{
					oct_data_new[i] += oct_data[mapper[i]];
				}
			}

			/**<Update the connectivity and write the para_tree.*/
			pablo116.updateConnectivity();
			pablo116.writeTest("Pablo116_iter"+to_string(iter), oct_data_new);

			oct_data = oct_data_new;
		}

#if NOMPI==0
		/**<PARALLEL TEST: (Load)Balance the octree over the processes with communicating the data.
		 * Preserve the family compact up to 4 levels over the max deep reached in the octree.*/
		uint8_t levels = 4;
		User_Data_LB<vector<double> > data_lb(oct_data);
		pablo116.loadBalance(data_lb, levels);
#endif

		/**<Update the connectivity and write the para_tree.*/
		pablo116.updateConnectivity();
		pablo116.writeTest("Pablo116_iter"+to_string(iter), oct_data);

#if NOMPI==0
	}

	MPI::Finalize();
#endif
}
