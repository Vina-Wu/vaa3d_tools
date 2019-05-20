#include "BrainVesselCPR_sampleplane.h"

Coor3D operator - (const Coor3D &n1, const Coor3D &n2)
{
    Coor3D res;
    res.x = n1.x - n2.x;
    res.y = n1.y - n2.y;
    res.z = n1.z - n2.z;
    return res;
}

Coor3D operator + (const Coor3D &n1, const Coor3D &n2)
{
    Coor3D res;
    res.x = n1.x + n2.x;
    res.y = n1.y + n2.y;
    res.z = n1.z + n2.z;
    return res;
}

Coor3D operator * (const Coor3D &n1, const double &n2)
{
    Coor3D res;
    res.x = n1.x * n2;
    res.y = n1.y * n2;
    res.z = n1.z * n2;
    return res;
}

Coor3D operator / (const Coor3D &n1, const double &n2)
{
    Coor3D res;
    res.x = n1.x / n2;
    res.y = n1.y / n2;
    res.z = n1.z / n2;
    return res;
}

Coor3D normCoor3D(const Coor3D &v)
{
    Coor3D res;
    double len = v.x * v.x + v.y * v.y + v.z * v.z;
    res = v / len;
    return res;
}

unsigned short int interpolation(Coor3D coor, unsigned short int * data1d, V3DLONG x_len, V3DLONG y_len, V3DLONG z_len)
{
    double x = coor.x;
    double y = coor.y;
    double z = coor.z;
    V3DLONG x0 = floor(x);
    V3DLONG y0 = floor(y);
    V3DLONG z0 = floor(z);

    unsigned short int value;

    if(x0<0||y0<0||z0<0||x0>x_len-1||y0>y_len-1||z0>z_len-1)
    {
        value = 0;
    }
    else
    {
        double xd = x-x0;
        double yd = y-y0;
        double zd = z-z0;
        V3DLONG x1 = MIN(x_len-1, x0+1);
        V3DLONG y1 = MIN(y_len-1, y0+1);
        V3DLONG z1 = MIN(z_len-1, z0+1);
        x0 = MAX(0,x0);
        y0 = MAX(0,y0);
        z0 = MAX(0,z0);

        unsigned short int c000 = data1d[x0+y0*x_len+z0*x_len*y_len];
        unsigned short int c001 = data1d[x0+y0*x_len+z1*x_len*y_len];
        unsigned short int c010 = data1d[x0+y1*x_len+z0*x_len*y_len];
        unsigned short int c011 = data1d[x0+y1*x_len+z1*x_len*y_len];
        unsigned short int c100 = data1d[x1+y0*x_len+z0*x_len*y_len];
        unsigned short int c101 = data1d[x1+y0*x_len+z1*x_len*y_len];
        unsigned short int c110 = data1d[x1+y1*x_len+z0*x_len*y_len];
        unsigned short int c111 = data1d[x1+y1*x_len+z1*x_len*y_len];

        double c00 = c000*(1-xd) + c100*xd;
        double c01 = c001*(1-xd) + c101*xd;
        double c10 = c010*(1-xd) + c110*xd;
        double c11 = c011*(1-xd) + c111*xd;

        double c0 = c00*(1-yd) + c10*yd;
        double c1 = c01*(1-yd) + c11*yd;

        double c = c0*(1-zd)+ c1*zd;
        value = (unsigned short int)c;
    }
    return value;

}

unsigned short int * samplePlane(unsigned short int * data1d, vector<Coor3D> centerline, V3DLONG x_len, V3DLONG y_len, V3DLONG z_len, int radius)
{
    Coor3D t; // tangent vector
    Coor3D n1; // normal vector 1
    Coor3D n2; // normal vector 2

    int center_num = centerline.size();
    Coor3D * tangent = new Coor3D[center_num]; //centerline tangent vector
    Coor3D * normal1 = new Coor3D[center_num];
    Coor3D * normal2 = new Coor3D[center_num];


    // calculate tangent vector at the first center point
    t = centerline[1] - centerline[0];
    t = normCoor3D(t);
    tangent[0] = t;


    // straight line flag
    int straight_count = 1;

    double len;

    // calculate tangent vector for 2rd to last-1 center point
    for(int i=1;i<centerline.size()-1;i++)
    {
        t = normCoor3D(centerline[i+1] - centerline[i-1]);
        tangent[i] = t;
        t = (centerline[i+1] + centerline[i-1]) / 2 - centerline[i];
        len = sqrt(t.x * t.x + t.y * t.y + t.z * t.z);
        if(!len)
        {
            straight_count++;
            //t = normal1[i-1];
        }
        else
        {
            t = t / len;
            normal1[i] = t;
            while(straight_count)
            {
                normal1[i-straight_count] = normal1[i];
                //straight_count = 0;
                straight_count--;
            }
        }
    }
    if(straight_count==center_num)
    {
        qDebug("straight line!!");
    }

    //process straight line near end point.
    while(straight_count)
    {
        normal1[center_num-straight_count-1] = normal1[center_num-straight_count-2];
        straight_count--;
    }

    //process last point.
    normal1[center_num-1] = normal1[center_num-2];

    //compute normal vector 2. functions came from matlab symbol calculation.
    for(int i=0;i<center_num;i++)
    {
        normal2[i].x =
          sqrt(1/(normal1[i].x*normal1[i].x*tangent[i].y*tangent[i].y + normal1[i].x*normal1[i].x*tangent[i].z*tangent[i].z
                  - 2*normal1[i].x*normal1[i].y*tangent[i].x*tangent[i].y - 2*normal1[i].x*normal1[i].z*tangent[i].x*tangent[i].z
                  + normal1[i].y*normal1[i].y*tangent[i].x*tangent[i].x + normal1[i].y*normal1[i].y*tangent[i].z*tangent[i].z
                  - 2*normal1[i].y*normal1[i].z*tangent[i].y*tangent[i].z + normal1[i].z*normal1[i].z*tangent[i].x*tangent[i].x
                  + normal1[i].z*normal1[i].z*tangent[i].y*tangent[i].y))*(normal1[i].y*tangent[i].z - normal1[i].z*tangent[i].y);


        normal2[i].y =
         -sqrt(1/(normal1[i].x*normal1[i].x*tangent[i].y*tangent[i].y + normal1[i].x*normal1[i].x*tangent[i].z*tangent[i].z
                  - 2*normal1[i].x*normal1[i].y*tangent[i].x*tangent[i].y - 2*normal1[i].x*normal1[i].z*tangent[i].x*tangent[i].z
                  + normal1[i].y*normal1[i].y*tangent[i].x*tangent[i].x + normal1[i].y*normal1[i].y*tangent[i].z*tangent[i].z
                  - 2*normal1[i].y*normal1[i].z*tangent[i].y*tangent[i].z + normal1[i].z*normal1[i].z*tangent[i].x*tangent[i].x
                  + normal1[i].z*normal1[i].z*tangent[i].y*tangent[i].y))*(normal1[i].x*tangent[i].z - normal1[i].z*tangent[i].x);


        normal2[i].z =
          sqrt(1/(normal1[i].x*normal1[i].x*tangent[i].y*tangent[i].y + normal1[i].x*normal1[i].x*tangent[i].z*tangent[i].z
                  - 2*normal1[i].x*normal1[i].y*tangent[i].x*tangent[i].y - 2*normal1[i].x*normal1[i].z*tangent[i].x*tangent[i].z
                  + normal1[i].y*normal1[i].y*tangent[i].x*tangent[i].x + normal1[i].y*normal1[i].y*tangent[i].z*tangent[i].z
                  - 2*normal1[i].y*normal1[i].z*tangent[i].y*tangent[i].z + normal1[i].z*normal1[i].z*tangent[i].x*tangent[i].x
                  + normal1[i].z*normal1[i].z*tangent[i].y*tangent[i].y))*(normal1[i].x*tangent[i].y - normal1[i].y*tangent[i].x);
    }

    //compute coordinate for each pixel to be sampled. result saved as an 1d vector.
    V3DLONG totalpxl_sample = (radius * 2 + 1) * (radius * 2 + 1) * center_num;
    Coor3D * sample_coor = new Coor3D[totalpxl_sample];
    //each slice is a cross section.
    int winlen = radius * 2 + 1;
    V3DLONG cur_id;
    Coor3D bias;
    for(int iz=0; iz<center_num; iz++)
    {
        for(int iy=0;iy<winlen;iy++)
        {
            for(int ix=0;ix<winlen;ix++)
            {
                cur_id = ix + iy*winlen + iz*winlen*winlen;
                bias = normal1[iz] * (ix - radius) + normal2[iz] * (iy - radius);
                sample_coor[cur_id] = centerline[iz] + bias;
            }
        }
    }

    //interpolation
    unsigned short int * data1d_sample = new unsigned short int[totalpxl_sample];
    for(int i=0;i<totalpxl_sample;i++)
    {
        data1d_sample[i] = interpolation(sample_coor[i], data1d, x_len, y_len, z_len);
    }
    return data1d_sample;
}

