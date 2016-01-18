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
		/**<Instantation of a 2D para_tree object.*/
		Class_Para_Tree<2> pablo2;

		/**<Set 2:1 balance only through faces.*/
		pablo2.setBalanceCodimension(1);
		uint32_t idx=0;
		pablo2.setBalance(idx,true);

		/**<Compute the connectivity and write the para_tree.*/
		pablo2.computeConnectivity();
		pablo2.write("Pablo2_iter0");

		/**<Refine globally two level and write the para_tree.*/
		for (int iter=1; iter<3; iter++){
			pablo2.adaptGlobalRefine();
			pablo2.updateConnectivity();
			pablo2.write("Pablo2_iter"+to_string(iter));
		}

		/**<Define a center point and a radius.*/
		double xc, yc;
		xc = yc = 0.5;
		double radius = 0.4;

		/**<Simple adapt() 6 times the octants with at least one node inside the circle.*/
		for (int iter=3; iter<9; iter++){
			uint32_t nocts = pablo2.getNumOctants();
			for (int i=0; i<nocts; i++){
				/**<Compute the nodes of the octant.*/
				vector<vector<double> > nodes = pablo2.getNodes(i);
				for (int j=0; j<global2D.nnodes; j++){
					double x = nodes[j][0];
					double y = nodes[j][1];
					if ((pow((x-xc),2.0)+pow((y-yc),2.0) <= pow(radius,2.0))){
						pablo2.setMarker(i, 1);
					}
				}
			}
			/**<Adapt octree.*/
			pablo2.adapt();

			/**<Update the connectivity and write the para_tree.*/
			pablo2.updateConnectivity();
			pablo2.write("Pablo2_iter"+to_string(iter));
		}
#if NOMPI==0
	}

	MPI::Finalize();
#endif
}

