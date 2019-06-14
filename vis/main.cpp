#include <kvs/Message>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/ValueArray>
#include <kvs/ColorMap>
#include <kvs/PolygonObject>
#include <kvs/OrthoSlice>
#include <kvs/Isosurface>
#include <kvs/StructuredVolumeObject>
#include <kvs/ExternalFaces>
#include <kvs/RayCastingRenderer>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <kvs/LineObject>

//////////////////////////
/////build coordinate/////
//////////////////////////

const size_t N = 4;

const size_t L = 3;

kvs::Real32 CoordArray[ N * 3 ] = {
    0.0, 0.0, 0.0,  // oringin (0.0, 0.0, 0.0)
    100.0, 0.0, 0.0,  // point x (100.0, 0.0, 0.0
    0.0, 100.0, 0.0,  // point y (0.0, 100.0, 0.0)
    0.0, 0.0, 100.0   // point z (0.0, 0.0, 100.0)
};

kvs::UInt8 ColorArray[ N * 3 ] = {
    255,   0,   0,  //x axis red
    0,   255,   0,  //y axis gren
    0,   0,   255   //z axis blue
};

kvs::UInt32 ConnectionArray[ L * 2 ] = {
    0, 1,  // x axis
    2, 0,  // y axis
    3, 0,  // z axis
};


kvs::LineObject* CreateLineObject()
{
    kvs::ValueArray<kvs::Real32> coords( CoordArray, N * 3 );
    kvs::ValueArray<kvs::UInt8> colors( ColorArray, N * 3 );
    kvs::ValueArray<kvs::UInt32> connections( ConnectionArray, L * 2 );
    
    kvs::LineObject* coordinate = new kvs::LineObject();
    coordinate->setCoords( coords );
    coordinate->setColors( colors );
    coordinate->setConnections( connections );
    coordinate->setSize( 10 );
    coordinate->setLineType( kvs::LineObject::Segment );
    coordinate->setColorType( kvs::LineObject::LineColor );
    
    return coordinate;
}
using namespace std;

//////////////////////
/////main program/////
//////////////////////

int main( int argc, char** argv )
{
    //read data
    int i;
    FILE*fp;
    size_t size = 32*32*92;
    if((fp=fopen("../src/fort.21","rb"))==NULL)  //nloop = 500 pressure
    {
        fprintf(stderr,"Can not open output file.\n");
        return 0;
    }
    
    fseek(fp,4,SEEK_SET);
    std::vector<float> v;
    for(i=0;i<size;i++)
    {
        float tmp = 0;
        fread((void*)(&tmp), 4, 1, fp);
        v.push_back(tmp);
    }
    fclose(fp);
    
    if((fp=fopen("../src/fort.23","rb"))==NULL)  //nloop = 1000 presure
    {
        fprintf(stderr,"Can not open output file.\n");
        return 0;
    }
    
    fseek(fp,4,SEEK_SET);
    std::vector<float> v1;
    for(i=0;i<size;i++)
    {
        float tmp = 0;
        fread((void*)(&tmp), 4, 1, fp);
        v1.push_back(tmp);
    }
    fclose(fp);
    if((fp=fopen("../src/fort.22","rb"))==NULL) //nloop = 500 enstrophy
    {
        fprintf(stderr,"Can not open output file.\n");
        return 0;
    }
    
    fseek(fp,4,SEEK_SET);
    std::vector<float> v2;
    for(i=0;i<size;i++)
    {
        float tmp = 0;
        fread((void*)(&tmp), 4, 1, fp);
        v2.push_back(tmp);
    }
    fclose(fp);
        //create object
    kvs::ValueArray<float> values( v );
    kvs::ValueArray<float> values1( v1 );
    kvs::ValueArray<float> values2( v2 );
    //nloop = 500 pressure
    kvs::StructuredVolumeObject* object = new kvs::StructuredVolumeObject();
    object->setGridType( kvs::StructuredVolumeObject::Uniform );
    object->setVeclen(1);
    object->setResolution( kvs::Vector3ui(92, 32, 32));
    object->setValues( kvs::AnyValueArray( values ));
    object->updateMinMaxValues();
    //nloop = 1000 pressure
    kvs::StructuredVolumeObject* object1 = new kvs::StructuredVolumeObject();
    object1->setGridType( kvs::StructuredVolumeObject::Uniform );
    object1->setVeclen(1);
    object1->setResolution( kvs::Vector3ui(92, 32, 32));
    object1->setValues( kvs::AnyValueArray( values1 ));
    object1->updateMinMaxValues();
    //nloop = 500 enstrophy
    kvs::StructuredVolumeObject* object2 = new kvs::StructuredVolumeObject();
    object2->setGridType( kvs::StructuredVolumeObject::Uniform );
    object2->setVeclen(1);
    object2->setResolution( kvs::Vector3ui(92, 32, 32));
    object2->setValues( kvs::AnyValueArray( values2 ));
    object2->updateMinMaxValues();

    kvs::glut::Application app( argc, argv );
    //create orthoslice
    const float p = object->resolution().y() * 0.5f;
    const kvs::OrthoSlice::AlignedAxis a = kvs::OrthoSlice::YAxis;
    const kvs::TransferFunction t( 256 );
    kvs::PolygonObject* object_or = new kvs::OrthoSlice( object, p, a, t );
    const float p1 = object1->resolution().z() * 0.5f;
    const kvs::OrthoSlice::AlignedAxis a1 = kvs::OrthoSlice::ZAxis;
    kvs::PolygonObject* object_or1 = new kvs::OrthoSlice( object1, p1, a1, t );
    //creatte isosurface
    const double i0 = ( object->maxValue() + object->minValue() ) * 0.5;
    const kvs::PolygonObject::NormalType n = kvs::PolygonObject::VertexNormal;
    const bool d = false;
    kvs::ColorMap cmap( 128 );
    cmap.create();
    kvs::TransferFunction t1( cmap );
    kvs::PolygonObject* object_iso = new kvs::Isosurface( object, i0, n, d, t );
    kvs::PolygonObject* object_iso1 = new kvs::Isosurface( object1, i0, n, d, t1 );
    kvs::LineObject* coordinate0 = CreateLineObject();
    kvs::LineObject* coordinate1 = CreateLineObject();
    kvs::LineObject* coordinate2 = CreateLineObject();
    //renderer setting
    kvs::Real32 sampling_step = 0.5f;
    kvs::Real32 opaque_value = 0.97f;
    kvs::TransferFunction tfunc( 256 );
    kvs::glsl::RayCastingRenderer* renderer = new kvs::glsl::RayCastingRenderer();
    renderer->setTransferFunction( tfunc );
    renderer->setSamplingStep( sampling_step );
    renderer->setOpaqueValue( opaque_value );
    //screen setting
    kvs::glut::Screen screen0( &app );
    screen0.setTitle("orthorslice" );
    screen0.setGeometry( 0, 0, 512, 512 );
    screen0.show();
    kvs::glut::Screen screen1( &app );
    screen1.setTitle("isosurface" );
    screen1.setGeometry( 512, 0, 512, 512 );
    screen1.show();
    kvs::glut::Screen screen2( &app );
    screen2.setTitle("rendering" );
    screen2.setGeometry( 1024, 0, 512, 512 );
    screen2.show();
    screen0.registerObject( coordinate0 );
    screen0.registerObject( object_or );
    screen0.registerObject( object_or1 );
    screen1.registerObject( coordinate1 );
    screen1.registerObject( object_iso );
    screen1.registerObject( object_iso1 );
    screen2.registerObject( object2, renderer );
    screen2.registerObject( coordinate2 );
    
    return app.run();
}
