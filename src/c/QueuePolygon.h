class QueuePolygon
{
public:
	size_t numFaces;
	size_t numVerts;
	size_t numNormals;
	int frame;
	int label;
	int track;

	QueuePolygon(size_t numFaces, size_t numVerts, size_t numNormals, int frame, int label, int track);
	~QueuePolygon();

	void QueuePolygon::setfaceData(double* faceData);
	double* QueuePolygon::getfaceData();
	void QueuePolygon::setvertData(double* vertData);
	double* QueuePolygon::getvertData();
	void QueuePolygon::setnormData(double* normData);
	double* QueuePolygon::getnormData();
	void QueuePolygon::setcolorData(double* colorData);
	double* QueuePolygon::getcolorData();

private:
	double* faceData;
	double* vertData;
	double* normData;
	double* colorData;
};