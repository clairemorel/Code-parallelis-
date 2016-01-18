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
		/**<Instantation of a 2D para_tree object.*/
		Class_Para_Tree<2> pablo3;

		/**<Set NO 2:1 balance for ancestor octant.*/
		uint32_t idx=0;
		pablo3.setBalance(idx,false);

		/**<Compute the connectivity and write the para_tree.*/
		pablo3.computeConnectivity();
		pablo3.write("Pablo3_iter"+to_string(iter));

		/**<Refine globally two level and write the para_tree.*/
		for (iter=1; iter<3; iter++){
			pablo3.adaptGlobalRefine();
			pablo3.updateConnectivity();
			pablo3.write("Pablo3_iter"+to_string(iter));
		}

		/**<Define a center point and a radius.*/
		double xc, yc;
		xc = yc = 0.5;
		double radius = 0.4;

		/**<Simple adapt() 6 times the octants with at least one node inside the circle.*/
		for (iter=3; iter<9; iter++){
			uint32_t nocts = pablo3.getNumOctants();
			for (int i=0; i<nocts; i++){
				/**<Compute the nodes of the octant.*/
				vector<vector<double> > nodes = pablo3.getNodes(i);
				for (int j=0; j<global2D.nnodes; j++){
					double x = nodes[j][0];
					double y = nodes[j][1];
					if ((pow((x-xc),2.0)+pow((y-yc),2.0) <= pow(radius,2.0))){
						pablo3.setMarker(i, 1);
					}
				}
			}
			/**<Adapt octree.*/
			pablo3.adapt();

			/**<Update the connectivity and write the para_tree.*/
			pablo3.updateConnectivity();
			pablo3.write("Pablo3_iter"+to_string(iter));
		}

		/**<Coarse globally one level and write the para_tree.*/
		pablo3.adaptGlobalCoarse();
		pablo3.updateConnectivity();
		pablo3.write("Pablo3_iter"+to_string(iter));

		/**<Define a center point and a radius.*/
		xc = yc = 0.35;
		radius = 0.15;

		/**<Simple adapt() 5 times the octants with at least one node inside the circle.*/
		for (iter=10; iter<15; iter++){
			uint32_t nocts = pablo3.getNumOctants();
			for (int i=0; i<nocts; i++){
				/**<Compute the nodes of the octant.*/
				vector<vector<double> > nodes = pablo3.getNodes(i);
				for (int j=0; j<global2D.nnodes; j++){
					double x = nodes[j][0];
					double y = nodes[j][1];
					/**<Set refinement marker=-1 (coarse it one time) for octants inside a circle.*/
					if ((pow((x-xc),2.0)+pow((y-yc),2.0) <= pow(radius,2.0))){
						pablo3.setMarker(i, -1);
					}
				}
			}
			/**<Adapt octree, update connectivity and write.*/
			pablo3.adapt();
			pablo3.updateConnectivity();
			pablo3.write("Pablo3_iter"+to_string(iter));
		}
#if NOMPI==0
	}
	MPI::Finalize();
#endif
}

