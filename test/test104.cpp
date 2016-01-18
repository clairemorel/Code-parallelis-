#include "preprocessor_defines.dat"
#include "Class_Global.hpp"
#include "Class_Para_Tree.hpp"

using namespace std;

// =================================================================================== //

int main(int argc, char *argv[]) {

#if NOMPI==0
	MPI::Init(argc, argv);

	{
#endif
		int iter = 0;
		int dim = 3;

		/**<Instantation of a 3D para_tree object.*/
		Class_Para_Tree<3> pablo104;

		/**<Refine globally four level and write the para_tree.*/
		for (iter=1; iter<5; iter++){
			pablo104.adaptGlobalRefine();
		}

		/**<Define a center point and a radius.*/
		double xc, yc;
		xc = yc = 0.5;
		double radius = 0.25;

		/**<Define vectors of data.*/
		uint32_t nocts = pablo104.getNumOctants();
		vector<double> oct_data(nocts, 0.0);

		/**<Assign a data to the octants with at least one node inside the cylinder.*/
		for (int i=0; i<nocts; i++){
			/**<Compute the nodes of the octant.*/
			vector<vector<double> > nodes = pablo104.getNodes(i);
			for (int j=0; j<global3D.nnodes; j++){
				double x = nodes[j][0];
				double y = nodes[j][1];
				if ((pow((x-xc),2.0)+pow((y-yc),2.0) <= pow(radius,2.0))){
					oct_data[i] = 1.0;
				}
			}
		}

		/**<Update the connectivity and write the para_tree.*/
		iter = 0;
		pablo104.updateConnectivity();
		pablo104.writeTest("Pablo104_iter"+to_string(iter), oct_data);

		/**<Smoothing iterations on initial data*/
		int start = 1;
		for (iter=start; iter<start+25; iter++){
			vector<double> oct_data_smooth(nocts, 0.0);
			vector<uint32_t> neigh, neigh_t;
			vector<bool> isghost, isghost_t;
			uint8_t iface, codim, nfaces;
			for (int i=0; i<nocts; i++){
				neigh.clear();
				isghost.clear();

				/**<Find neighbours through faces (codim=1), edges (codim=2) and nodes (codim=3) of the octants*/
				for (codim=1; codim<dim+1; codim++){
					if (codim == 1){
						nfaces = global3D.nfaces;
					}
					else if (codim == 2){
						nfaces = global3D.nedges;
					}
					else if (codim == 3){
						nfaces = global3D.nnodes;
					}
					for (iface=0; iface<nfaces; iface++){
						pablo104.findNeighbours(i,iface,codim,neigh_t,isghost_t);
						neigh.insert(neigh.end(), neigh_t.begin(), neigh_t.end());
						isghost.insert(isghost.end(), isghost_t.begin(), isghost_t.end());
					}
				}

				/**<Smoothing data with the average over the one ring neighbours of octants*/
				oct_data_smooth[i] = oct_data[i]/(neigh.size()+1);
				for (int j=0; j<neigh.size(); j++){
					if (isghost[j]){
						/**< Do nothing - No ghosts: is a serial test.*/
					}
					else{
						oct_data_smooth[i] += oct_data[neigh[j]]/(neigh.size()+1);
					}
				}
			}

			/**<Update the connectivity and write the para_tree.*/
			pablo104.updateConnectivity();
			pablo104.writeTest("Pablo104_iter"+to_string(iter), oct_data_smooth);

			oct_data = oct_data_smooth;
		}
#if NOMPI==0
	}

	MPI::Finalize();
#endif
}

