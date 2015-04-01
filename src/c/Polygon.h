class Polygon
{
public:
	double* faceData;
	double* vertData;
	double* normData;
	double* colorData;
	size_t numFaces;
	size_t numVerts;
	int frame;
	// Two arrays for labels and tracks

	Polygon(size_t numFaces, size_t numVerts);
	~Polygon();
};