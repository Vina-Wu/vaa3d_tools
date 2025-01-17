/* imageblend.cpp
 * 2011-07-30: the program is created by Yang Yu
 * 2011-08-10: updated by Hanchuan Peng
 * 2012-01-30: adjust the whole process by Yang Yu
 */


#ifdef USE_Qt5
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include <cmath>
#include <ctime>
#include <vector>
#include <cstdlib>
#include <iostream>

#include "basic_surf_objs.h"
#include "stackutil.h"
#include "volimg_proc.h"
#include "img_definition.h"
#include "basic_landmark.h"

#include "mg_utilities.h"
#include "mg_image_lib.h"

#include "basic_landmark.h"
#include "basic_4dimage.h"

#include "../istitch/y_imglib.h"

#include "imageblend.h"
#include "y_lsminfo.h"

using namespace std;

#define EMPTY 1

#define SAMPLERATIO 0.3
#define CUBEFACTOR 2

#ifdef WIN32
#define SEPCHAR '\\'
#else
#define SEPCHAR '/'
#endif

//
Q_EXPORT_PLUGIN2(blend_multiscanstacks, ImageBlendPlugin);

//
typedef struct
{
    REAL dx, dy, dz;
    REAL coef;
}TransStatistics;

// color lut
int getChannelNum(int r, int g, int b)
{
    if(r==255 && g==255 && b==255)
    {
        return 3; // gray
    }
    else if(r==255 && g==0 && b==0)
    {
        return 0; // red
    }
    else if(r==0 && g==255 && b==0)
    {
        return 1; // green
    }
    else if(r==0 && g==0 && b==255)
    {
        return 2; // blue
    }
    else
    {
        cout << "Error: unsopported color"<<endl;
        return -1;
    }
}

//
template <class Tdata>
void img_cutting(Tdata *pInput, V3DLONG *szInput, Tdata* &pOutput, V3DLONG* szOutput)
{
    // cutting blank plane occuring in image boundaries
    V3DLONG pz=0, nz=0, py=0, ny=0, px=0, nx=0;
    V3DLONG start_z=0;
    V3DLONG end_z=szInput[2]-1;
    V3DLONG start_y=0;
    V3DLONG end_y=szInput[1]-1;
    V3DLONG start_x=0;
    V3DLONG end_x=szInput[0]-1;

    V3DLONG colordim = szInput[3];

    V3DLONG pagesz = szInput[0]*szInput[1]*szInput[2];

    bool b_blankplanefound;

    // find NULL channel first
    V3DLONG nullc = -1;
    for(V3DLONG c=0; c<szInput[3]; c++) // image 1
    {
        V3DLONG offset_c = c*pagesz;
        V3DLONG sumint = 0;
        for (V3DLONG k=0; k<szInput[2]; k++)
        {
            V3DLONG offset_k = offset_c + k*szInput[0]*szInput[1];
            for(V3DLONG j=0; j<szInput[1]; j++)
            {
                V3DLONG offset_j = offset_k + j*szInput[0];
                for(V3DLONG i=0; i<szInput[0]; i++)
                {
                    V3DLONG idx = offset_j + i;

                    sumint += pInput[idx];
                }
            }
        }

        if(sumint<EMPTY)
        {
            nullc = c;
        }
    }

    // cutting
    // along z +
    b_blankplanefound=true;
    while (b_blankplanefound)
    {
        double sum;

        for(V3DLONG k=0; k<szInput[2]; k++)
        {
            V3DLONG offset_k = k*szInput[1]*szInput[0];

            b_blankplanefound=false;
            for(V3DLONG c=0; c<colordim; c++)
            {
                sum = 0;

                if(c==nullc) continue;

                V3DLONG offset_c = c*pagesz + offset_k;
                for(V3DLONG j=0; j<szInput[1]; j++)
                {
                    V3DLONG offset_j = offset_c + j*szInput[0];
                    for(V3DLONG i=0; i<szInput[0]; i++)
                    {
                        sum += pInput[offset_j + i];
                    }
                }

                if(sum<EMPTY)
                {
                    b_blankplanefound = true;
                }
            }

            if(b_blankplanefound)
            {
                pz++;
            }
            else
            {
                break;
            }
        }
        b_blankplanefound=false;
    }

    // along z -
    b_blankplanefound=true;
    while (b_blankplanefound)
    {
        double sum;

        for(V3DLONG k=szInput[2]-1; k>0; k--)
        {
            V3DLONG offset_k = k*szInput[1]*szInput[0];

            b_blankplanefound=false;
            for(V3DLONG c=0; c<colordim; c++)
            {
                sum = 0;

                if(c==nullc) continue;

                V3DLONG offset_c = c*pagesz + offset_k;
                for(V3DLONG j=0; j<szInput[1]; j++)
                {
                    V3DLONG offset_j = offset_c + j*szInput[0];
                    for(V3DLONG i=0; i<szInput[0]; i++)
                    {
                        sum += pInput[offset_j + i];
                    }
                }

                if(sum<EMPTY)
                {
                    b_blankplanefound = true;
                }
            }

            if(b_blankplanefound)
            {
                nz++;
            }
            else
            {
                break;
            }
        }
        b_blankplanefound=false;
    }

    // along y +
    b_blankplanefound=true;
    while (b_blankplanefound)
    {
        double sum;

        for(V3DLONG j=0; j<szInput[1]; j++)
        {
            V3DLONG offset_j = j*szInput[0];

            b_blankplanefound=false;
            for(V3DLONG c=0; c<colordim; c++)
            {
                sum = 0;

                if(c==nullc) continue;

                V3DLONG offset_c = c*pagesz + offset_j;
                for(V3DLONG k=0; k<szInput[2]; k++)
                {
                    V3DLONG offset_k = offset_c + k*szInput[1]*szInput[0];

                    for(V3DLONG i=0; i<szInput[0]; i++)
                    {
                        sum += pInput[offset_k + i];
                    }
                }

                if(sum<EMPTY)
                {
                    b_blankplanefound = true;
                }
            }

            if(b_blankplanefound)
            {
                py++;
            }
            else
            {
                break;
            }
        }
        b_blankplanefound=false;
    }

    // along y -
    b_blankplanefound=true;
    while (b_blankplanefound)
    {
        double sum;

        for(V3DLONG j=szInput[1]-1; j>0; j--)
        {
            V3DLONG offset_j = j*szInput[0];

            b_blankplanefound=false;
            for(V3DLONG c=0; c<colordim; c++)
            {
                sum = 0;

                if(c==nullc) continue;

                V3DLONG offset_c = c*pagesz + offset_j;
                for(V3DLONG k=0; k<szInput[2]; k++)
                {
                    V3DLONG offset_k = offset_c + k*szInput[1]*szInput[0];

                    for(V3DLONG i=0; i<szInput[0]; i++)
                    {
                        sum += pInput[offset_k + i];
                    }
                }

                if(sum<EMPTY)
                {
                    b_blankplanefound = true;
                }
            }

            if(b_blankplanefound)
            {
                ny++;
            }
            else
            {
                break;
            }
        }
        b_blankplanefound=false;
    }

    // along x +
    b_blankplanefound=true;
    while (b_blankplanefound)
    {
        double sum;

        for(V3DLONG i=0; i<szInput[0]; i++)
        {
            b_blankplanefound=false;
            for(V3DLONG c=0; c<colordim; c++)
            {
                sum = 0;

                if(c==nullc) continue;

                V3DLONG offset_c = c*pagesz;
                for(V3DLONG k=0; k<szInput[2]; k++)
                {
                    V3DLONG offset_k = offset_c + k*szInput[1]*szInput[0];
                    for(V3DLONG j=0; j<szInput[1]; j++)
                    {
                        V3DLONG offset_j = offset_k + j*szInput[0];

                        sum += pInput[offset_j + i];
                    }
                }

                if(sum<EMPTY)
                {
                    b_blankplanefound = true;
                }
            }

            if(b_blankplanefound)
            {
                px++;
            }
            else
            {
                break;
            }
        }
        b_blankplanefound=false;
    }

    // along x -
    b_blankplanefound=true;
    while (b_blankplanefound)
    {
        double sum;

        for(V3DLONG i=szInput[0]-1; i>0; i--)
        {
            b_blankplanefound=false;
            for(V3DLONG c=0; c<colordim; c++)
            {
                sum = 0;

                if(c==nullc) continue;

                V3DLONG offset_c = c*pagesz;
                for(V3DLONG k=0; k<szInput[2]; k++)
                {
                    V3DLONG offset_k = offset_c + k*szInput[1]*szInput[0];
                    for(V3DLONG j=0; j<szInput[1]; j++)
                    {
                        V3DLONG offset_j = offset_k + j*szInput[0];

                        sum += pInput[offset_j + i];
                    }
                }

                if(sum<EMPTY)
                {
                    b_blankplanefound = true;
                }
            }

            if(b_blankplanefound)
            {
                nx++;
            }
            else
            {
                break;
            }
        }
        b_blankplanefound=false;
    }

    //
    start_x += px; start_y += py; start_z += pz;
    end_x -= nx; end_y -= ny; end_z -= nz;

    V3DLONG sx = end_x - start_x + 1;
    V3DLONG sy = end_y - start_y + 1;
    V3DLONG sz = end_z - start_z + 1;
    V3DLONG pagesz_new = sx*sy*sz;
    V3DLONG totalplxs = pagesz_new*colordim;

    // new image
    try {
        pOutput = new Tdata [totalplxs];

        for(V3DLONG c=0; c<colordim; c++)
        {
            V3DLONG offset_c_old = c*pagesz;
            V3DLONG offset_c_new = c*pagesz_new;
            for(V3DLONG k=start_z; k<=end_z; k++)
            {
                V3DLONG offset_k_old = offset_c_old + k*szInput[0]*szInput[1];
                V3DLONG offset_k_new = offset_c_new + (k-start_z)*sx*sy;
                for(V3DLONG j=start_y; j<=end_y; j++)
                {
                    V3DLONG offset_j_old = offset_k_old + j*szInput[0];
                    V3DLONG offset_j_new = offset_k_new + (j-start_y)*sx;
                    for(V3DLONG i=start_x; i<=end_x; i++)
                    {
                        pOutput[offset_j_new + i - start_x] = pInput[offset_j_old + i];
                    }
                }
            }
        }

        szOutput[0] = sx;
        szOutput[1] = sy;
        szOutput[2] = sz;
        szOutput[3] = colordim;

    }
    catch (...) {
        cout<<"Fail to allocate new image memory."<<endl;
        return;
    }
}

//
template <class Tdata, class Y_IMG_DATATYPE>
bool ref_aligning(Tdata *p1dImg1, Tdata *p1dImg2, V3DLONG *szImg, V3DLONG *&outputoffsets, REAL &coef)
{
    // two images with the same size
    V3DLONG sx = szImg[0];
    V3DLONG sy = szImg[1];
    V3DLONG sz = szImg[2];
    V3DLONG sc = szImg[3];

    V3DLONG pagesz_sub = sx*sy*sz;

    V3DLONG tx = szImg[0];
    V3DLONG ty = szImg[1];
    V3DLONG tz = szImg[2];
    V3DLONG tc = szImg[3];

    V3DLONG pagesz_tar = tx*ty*tz;

    // anchor position
    REAL pos_score = 0;
    V3DLONG pos_x = sx-1, pos_y = sy-1, pos_z = sz-1;
    V3DLONG sx_ori = sx, sy_ori = sy, sz_ori = sz;

    V3DLONG *szPad = NULL;
    V3DLONG *szTar = NULL;
    V3DLONG *szSub = NULL;
    REAL *scale = NULL;

    try
    {
        szPad = new V3DLONG [3];
        szTar = new V3DLONG [3];
        szSub = new V3DLONG [3];
        scale = new REAL [3];
    }
    catch (...)
    {
        cout<<"Fail to allocate memories."<<endl;
        return false;
    }

    // initial offset
    V3DLONG offset[3];
    offset[0] = 0;
    offset[1] = 0;
    offset[2] = 0;

    float overlap_percent = 0.1;

    V3DLONG nbbx, nbby, nbbz;

    scale[0] = SAMPLERATIO;
    scale[1] = SAMPLERATIO;
    scale[2] = SAMPLERATIO;

    nbbx = 1/scale[0] + 2;
    nbby = 1/scale[1] + 2;
    nbbz = 1/scale[2] + 2;

    V3DLONG bsx = (offset[0]>0)?0:-offset[0];
    V3DLONG bsy = (offset[1]>0)?0:-offset[1];
    V3DLONG bsz = (offset[2]>0)?0:-offset[2];

    if(bsx-nbbx>=0)
        bsx -= nbbx;
    else
        bsx = 0;

    if(bsy-nbby>=0)
        bsy -= nbby;
    else
        bsy = 0;

    if(bsz-nbbz>=0)
        bsz -= nbbz;
    else
        bsz = 0;

    V3DLONG esx = (sx-1);
    V3DLONG esy = (sy-1);
    V3DLONG esz = (sz-1);

    if(offset[0])
    {
        V3DLONG tmp = sx-1 + offset[0] - (tx-1);

        if(tmp>0)
            esx = sx-1 -tmp;
    }

    if(offset[1])
    {
        V3DLONG tmp = offset[1]+sy-1 - (ty-1);

        if(tmp>0)
            esy = sy-1 - tmp;
    }

    if(offset[2])
    {
        V3DLONG tmp = offset[2]+sz-1 - (tz-1);

        if(tmp>0)
            esz = sz-1 -tmp;
    }

    if(esx+nbbx<=sx-1)
        esx += nbbx;
    else
        esx = sx-1;

    if(esy+nbby<=sy-1)
        esy += nbby;
    else
        esy = sy-1;

    if(esz+nbbz<=sz-1)
        esz += nbbz;
    else
        esz = sz-1;

    V3DLONG btx = (offset[0]>0)?offset[0]:0;
    V3DLONG bty = (offset[1]>0)?offset[1]:0;
    V3DLONG btz = (offset[2]>0)?offset[2]:0;

    if(btx-nbbx>=0)
        btx -= nbbx;
    else
        btx = 0;

    if(bty-nbby>=0)
        bty -= nbby;
    else
        bty = 0;

    if(btz-nbbz>=0)
        btz -= nbbz;
    else
        btz = 0;

    V3DLONG etx = (offset[0]>0)?(tx-1):pos_x;
    V3DLONG ety = (offset[1]>0)?(ty-1):pos_y;
    V3DLONG etz = (offset[2]>0)?(tz-1):pos_z;

    if(offset[0]>0)
    {
        V3DLONG tmp = (tx-1) - (sx-1 + offset[0]);

        if(tmp>0)
            etx = tx-1 - tmp;
    }

    if(offset[1]>0)
    {
        V3DLONG tmp = (ty-1) - (sy-1 + offset[1]);

        if(tmp>0)
            ety = ty-1 - tmp;
    }

    if(offset[2]>0)
    {
        V3DLONG tmp = (tz-1) - (sz-1 + offset[2]);

        if(tmp>0)
            etz = tz-1 - tmp;
    }

    if(etx+nbbx<=tx-1)
        etx += nbbx;
    else
        etx = tx-1;

    if(ety+nbby<=ty-1)
        ety += nbby;
    else
        ety = ty-1;

    if(etz+nbbz<=tz-1)
        etz += nbbz;
    else
        etz = tz-1;

    V3DLONG bbsx = esx - bsx + 1; if(bbsx<0 || bbsx>sx) return -1;
    V3DLONG bbsy = esy - bsy + 1; if(bbsy<0 || bbsy>sy) return -1;
    V3DLONG bbsz = esz - bsz + 1; if(bbsz<0 || bbsz>sz) return -1;

    V3DLONG bbtx = etx - btx + 1; if(bbtx<0 || bbtx>tx) return -1;
    V3DLONG bbty = ety - bty + 1; if(bbty<0 || bbty>ty) return -1;
    V3DLONG bbtz = etz - btz + 1; if(bbtz<0 || bbtz>tz) return -1;

    float rate_x, rate_y, rate_z;

    rate_x = qMax( (float)bbsx/(float)sx, (float)bbtx/(float)tx);
    rate_y = qMax( (float)bbsy/(float)sy, (float)bbty/(float)ty);
    rate_z = qMax( (float)bbsz/(float)sz, (float)bbtz/(float)tz);

    // crop options
    // planes
    bool plane_yz=false, plane_xz=false, plane_xy=false;

    if(rate_x <= rate_y && rate_x <= rate_z)
    {
        plane_yz = true;

        bsy = 0; esy = sy-1;
        bbsy = sy;

        bty = 0; ety = ty-1;
        bbty = ty;

        bsz = 0; esz = sz-1;
        bbsz = sz;

        btz = 0; etz = tz-1;
        bbtz = tz;

    }
    else if(rate_y <= rate_x && rate_y <= rate_z)
    {
        plane_xz = true;

        bsx = 0; esx = sx-1;
        bbsx = sx;

        btx = 0; etx = tx-1;
        bbtx = tx;

        bsz = 0; esz = sz-1;
        bbsz = sz;

        btz = 0; etz = tz-1;
        bbtz = tz;

    }
    else if(rate_z <= rate_x && rate_z <= rate_y)
    {
        plane_xy = true;

        bsx = 0; esx = sx-1;
        bbsx = sx;

        btx = 0; etx = tx-1;
        bbtx = tx;

        bsy = 0; esy = sy-1;
        bbsy = sy;

        bty = 0; ety = ty-1;
        bbty = ty;

    }

    //
    //qDebug() << " overlap ratio ..." << rate_x << rate_y << rate_z << "planes yz xz xy ..." << plane_yz << plane_xz << plane_xy;
    //qDebug() << " current bounding box ... boundary ... "<< bsx << bsy << bsz << esx << esy << esz << btx << bty << btz << etx << ety << etz;
    //qDebug() << " current bounding box ... dims ... "<< bbsx << bbsy << bbsz << bbtx << bbty << bbtz;

    //
    REAL sum=0;
    for(V3DLONG k=bsz; k<=esz; k++)
    {
        V3DLONG offset_o_k = k*sx*sy;
        for(V3DLONG j=bsy; j<=esy; j++)
        {
            V3DLONG offset_o_j = offset_o_k + j*sx;
            for(V3DLONG i=bsx; i<=esx; i++)
            {
                V3DLONG idx_o = offset_o_j + i;

                sum += p1dImg1[idx_o];
            }
        }
    }
    Tdata meanv = (Tdata) (sum/(bbsx*bbsy*bbsz));

    // extract thick planes boundary bounding box
    //---------------------------------------------------------------------------------------------------------------
    if(plane_yz == true)
    {
        // finding rich information plane from sub along x
        V3DLONG info_count=0, xpln=0, max_info=0;

        // approach 1
        V3DLONG weights = bbsx*0.15;
        weights /= 2;
        V3DLONG start_x=bsx+weights+nbbx, end_x=esx-weights-nbbx;
        for(V3DLONG i=start_x; i<=end_x; i++) //
        {
            info_count = 0;

            for(V3DLONG k=bsz; k<=esz; k++)
            {
                V3DLONG offset_o_k = k*sx*sy;
                for(V3DLONG j=bsy; j<=esy; j++)
                {
                    V3DLONG idx_o = offset_o_k + j*sx + i;

                    if( p1dImg1[idx_o] > meanv)
                        info_count++;
                }
            }

            if(info_count > max_info)
            {
                max_info = info_count;
                xpln = i;
            }

        }

        if(xpln<start_x) xpln = start_x;

        //qDebug() << "xpln ..." << xpln;

        // extraction
        V3DLONG b_bsx = xpln - weights; //

        if(b_bsx>bsx)
            bsx = b_bsx;

        V3DLONG e_esx = xpln + weights;

        if(e_esx<esx)
            esx = e_esx;

        bbsx = esx-bsx+1; // dims

        // crop corresponding thick planes from tar image
        V3DLONG b_btx = bsx + offset[0] - nbbx;

        if(b_btx>btx)
            btx = b_btx;

        V3DLONG e_etx = esx + offset[0] + nbbx;

        if(e_etx<etx)
            etx = e_etx;

        bbtx = etx-btx+1;

        //qDebug() << " updated boundary ... "<< bsx << bsy << bsz << esx << esy << esz << btx << bty << btz << etx << ety << etz;
        //qDebug() << " updated dims ... "<< bbsx << bbsy << bbsz << bbtx << bbty << bbtz;

        V3DLONG pagesz_bb_sub = bbsx*bbsy*bbsz;
        V3DLONG pagesz_bb_tar = bbtx*bbty*bbtz;

        // extract one plane from sub
        Tdata* p_sub = new Tdata [pagesz_bb_sub];
        if (!p_sub)
        {
            printf("Fail to allocate memory.\n");
            return -1;
        }
        else
        {
            for(V3DLONG k=bsz; k<=esz; k++)
            {
                V3DLONG offset_k = (k-bsz)*bbsx*bbsy;
                V3DLONG offset_o_k = k*sx*sy;
                for(V3DLONG j=bsy; j<=esy; j++)
                {
                    V3DLONG offset_j = offset_k + (j-bsy)*bbsx;
                    V3DLONG offset_o_j = offset_o_k + j*sx;
                    for(V3DLONG i=bsx; i<=esx; i++)
                    {
                        V3DLONG idx = offset_j + (i-bsx);
                        V3DLONG idx_o = offset_o_j + i;

                        p_sub[idx] = p1dImg1[idx_o];
                    }
                }
            }
        }

        Tdata* p_tar = new Tdata [pagesz_bb_tar];
        if (!p_tar)
        {
            printf("Fail to allocate memory.\n");
            return -1;
        }

        //REAL max_response = 0;
        // search the best match plane from tar
        for(V3DLONG k=btz; k<=etz; k++)
        {
            V3DLONG offset_k = (k-btz)*bbtx*bbty;
            V3DLONG offset_o_k = k*tx*ty;
            for(V3DLONG j=bty; j<=ety; j++)
            {
                V3DLONG offset_j = offset_k + (j-bty)*bbtx;
                V3DLONG offset_o_j = offset_o_k + j*tx;
                for(V3DLONG i=btx; i<=etx; i++)
                {
                    V3DLONG idx = offset_j + (i-btx);
                    V3DLONG idx_o = offset_o_j + i;

                    p_tar[idx] = p1dImg2[idx_o];
                }
            }
        }

        //
        sx = bbsx; sy = bbsy; sz = bbsz;
        tx = bbtx; ty = bbty; tz = bbtz;

        V3DLONG sx_pad = sx+tx-1, sy_pad = sy+ty-1, sz_pad = sz+tz-1;

        V3DLONG even_odd = sx_pad%2; // 0 for even 1 for odd

        bool fftw_in_place = true;

        if(fftw_in_place)
            sx_pad += (2-even_odd); //2*(sx_pad/2 + 1); // fftw_in_place

        V3DLONG len_pad = sx_pad*sy_pad*sz_pad;

        szPad[0] = sx_pad;
        szPad[1] = sy_pad;
        szPad[2] = sz_pad;

        szTar[0] = tx;
        szTar[1] = ty;
        szTar[2] = tz;

        szSub[0] = sx;
        szSub[1] = sy;
        szSub[2] = sz;

        REAL* p_f_sub = new REAL [len_pad];
        if (!p_f_sub)
        {
            printf("Fail to allocate memory.\n");
            return -1;
        }
        else
        {
            // init
            memset(p_f_sub, 0, sizeof(REAL)*len_pad);

            //padding zeros for subject image
            Y_IMG_REAL pOut(p_f_sub, szPad);
            Y_IMG_DATATYPE pIn(p_sub, szSub);

            YImg<REAL, V3DLONG, Y_IMG_REAL, Y_IMG_DATATYPE> tmp;
            tmp.padding(pOut, pIn, true, fftw_in_place, even_odd, 3);

        }

        REAL* p_f_tar = new REAL [len_pad];
        if (!p_f_tar)
        {
            printf("Fail to allocate memory.\n");
            return -1;
        }
        else
        {
            // init
            memset(p_f_tar, 0, sizeof(REAL)*len_pad);

            //padding zeros for target image
            Y_IMG_REAL pOut(p_f_tar, szPad);
            Y_IMG_DATATYPE pIn(p_tar, szTar);

            YImg<REAL, V3DLONG, Y_IMG_REAL, Y_IMG_DATATYPE> tmp;
            tmp.padding(pOut, pIn, false, fftw_in_place, even_odd, 3);

        }

        //de-alloc
        if(p_sub) {delete []p_sub; p_sub=0;}
        if(p_tar) {delete []p_tar; p_tar=0;}

        // fft-ncc
        PEAKS pos_pcncc;

        pos_pcncc.x = (bsx - ((sx_ori-1) - pos_x)) - btx + (bbsx-1);
        pos_pcncc.y = pos_y;
        pos_pcncc.z = pos_z;

        //qDebug()<<" init translations ... "<<pos_pcncc.x<<pos_pcncc.y<<pos_pcncc.z;

        Y_IMG_REAL pOut(p_f_sub, szPad);
        Y_IMG_REAL pIn(p_f_tar, szPad);

        YImg<REAL, V3DLONG, Y_IMG_REAL, Y_IMG_REAL> tmp;
        tmp.fftnccp3D(pOut, pIn, szSub, szTar, even_odd, fftw_in_place, scale, &pos_pcncc); // consider +- 5 pixels offset

        pos_score = pos_pcncc.value;

        //qDebug() << " bb cross correlation ..." << pos_score;
        //qDebug() << " estimated translations ... "<<pos_pcncc.x<<pos_pcncc.y<<pos_pcncc.z;

        pos_x = (sx_ori-1) - (bsx - (btx + pos_pcncc.x - (bbsx-1))); pos_y = pos_pcncc.y; pos_z = pos_pcncc.z; //

        //de-alloc
        if(p_f_sub) {delete []p_f_sub; p_f_sub=0;}
        if(p_f_tar) {delete []p_f_tar; p_f_tar=0;}


    }
    else if(plane_xz == true)
    {
        // finding rich information plane from sub along y
        V3DLONG info_count=0, ypln, max_info=0;

        V3DLONG weights = bbsy*0.15;
        weights /= 2;
        V3DLONG start_y=bsy+weights+nbby, end_y=esy-weights-nbby;
        for(V3DLONG j=start_y; j<=end_y; j++) //
        {
            info_count = 0;

            V3DLONG offset_o_j = j*sx;
            for(V3DLONG k=bsz; k<=esz; k++)
            {
                V3DLONG offset_o_k = k*sx*sy;
                for(V3DLONG i=bsx; i<=esx; i++)
                {
                    V3DLONG idx_o = offset_o_k + offset_o_j + i;

                    if( p1dImg1[idx_o] > meanv)
                        info_count++;
                }
            }

            if(info_count > max_info)
            {
                max_info = info_count;
                ypln = j;
            }

        }

        //qDebug() << "plane ..." << ypln;

        // extraction
        V3DLONG b_bsy = ypln - weights;

        if(b_bsy>bsy)
            bsy = b_bsy;

        V3DLONG e_esy = ypln + weights;

        if(e_esy<esy)
            esy = e_esy;

        bbsy = esy-bsy+1; // dims

        // crop corresponding thick planes from tar image
        V3DLONG b_bty = bsy + offset[1] - nbby; //ypln + offset[1] - nbby;

        if(b_bty>bty)
            bty = b_bty;

        V3DLONG e_ety = esy + offset[1] + nbby; //ypln + offset[1] + nbby;

        if(e_ety<ety)
            ety = e_ety;

        bbty = ety-bty+1;

        //qDebug() << " updated boundary ... "<< bsx << bsy << bsz << esx << esy << esz << btx << bty << btz << etx << ety << etz;
        //qDebug() << " updated dims ... "<< bbsx << bbsy << bbsz << bbtx << bbty << bbtz;

        V3DLONG pagesz_bb_sub = bbsx*bbsy*bbsz;
        V3DLONG pagesz_bb_tar = bbtx*bbty*bbtz;

        // extract one plane from sub
        Tdata* p_sub = new Tdata [pagesz_bb_sub];
        if (!p_sub)
        {
            printf("Fail to allocate memory.\n");
            return -1;
        }
        else
        {
            for(V3DLONG k=bsz; k<=esz; k++)
            {
                V3DLONG offset_k = (k-bsz)*bbsx*bbsy;
                V3DLONG offset_o_k = k*sx*sy;
                for(V3DLONG j=bsy; j<=esy; j++)
                {
                    V3DLONG offset_j = offset_k + (j-bsy)*bbsx;
                    V3DLONG offset_o_j = offset_o_k + j*sx;
                    for(V3DLONG i=bsx; i<=esx; i++)
                    {
                        V3DLONG idx = offset_j + (i-bsx);
                        V3DLONG idx_o = offset_o_j + i;

                        p_sub[idx] = p1dImg1[idx_o];
                    }
                }
            }
        }

        Tdata* p_tar = new Tdata [pagesz_bb_tar];
        if (!p_tar)
        {
            printf("Fail to allocate memory.\n");
            return -1;
        }

        //REAL max_response = 0;
        // search the best match plane from tar
        for(V3DLONG k=btz; k<=etz; k++)
        {
            V3DLONG offset_k = (k-btz)*bbtx*bbty;
            V3DLONG offset_o_k = k*tx*ty;
            for(V3DLONG j=bty; j<=ety; j++)
            {
                V3DLONG offset_j = offset_k + (j-bty)*bbtx;
                V3DLONG offset_o_j = offset_o_k + j*tx;
                for(V3DLONG i=btx; i<=etx; i++)
                {
                    V3DLONG idx = offset_j + (i-btx);
                    V3DLONG idx_o = offset_o_j + i;

                    p_tar[idx] = p1dImg2[idx_o];
                }
            }
        }

        //
        sx = bbsx; sy = bbsy; sz = bbsz;
        tx = bbtx; ty = bbty; tz = bbtz;

        V3DLONG sx_pad = sx+tx-1, sy_pad = sy+ty-1, sz_pad = sz+tz-1;

        V3DLONG even_odd = sx_pad%2; // 0 for even 1 for odd

        bool fftw_in_place = true;

        if(fftw_in_place)
            sx_pad += (2-even_odd); //2*(sx_pad/2 + 1); // fftw_in_place

        V3DLONG len_pad = sx_pad*sy_pad*sz_pad;

        szPad[0] = sx_pad;
        szPad[1] = sy_pad;
        szPad[2] = sz_pad;

        szTar[0] = tx;
        szTar[1] = ty;
        szTar[2] = tz;

        szSub[0] = sx;
        szSub[1] = sy;
        szSub[2] = sz;

        REAL* p_f_sub = new REAL [len_pad];
        if (!p_f_sub)
        {
            printf("Fail to allocate memory.\n");
            return -1;
        }
        else
        {
            // init
            memset(p_f_sub, 0, sizeof(REAL)*len_pad);

            //padding zeros for sub image
            Y_IMG_REAL pOut(p_f_sub, szPad);
            Y_IMG_DATATYPE pIn(p_sub, szSub);

            YImg<REAL, V3DLONG, Y_IMG_REAL, Y_IMG_DATATYPE> tmp;
            tmp.padding(pOut, pIn, true, fftw_in_place, even_odd, 3);

        }

        REAL* p_f_tar = new REAL [len_pad];
        if (!p_f_tar)
        {
            printf("Fail to allocate memory.\n");
            return -1;
        }
        else
        {
            // init
            memset(p_f_tar, 0, sizeof(REAL)*len_pad);

            //padding zeros for tar image
            Y_IMG_REAL pOut(p_f_tar, szPad);
            Y_IMG_DATATYPE pIn(p_tar, szTar);

            YImg<REAL, V3DLONG, Y_IMG_REAL, Y_IMG_DATATYPE> tmp;
            tmp.padding(pOut, pIn, false, fftw_in_place, even_odd, 3);

        }

        //de-alloc
        if(p_sub) {delete []p_sub; p_sub=0;}
        if(p_tar) {delete []p_tar; p_tar=0;}


        // fft-ncc
        PEAKS pos_pcncc;

        pos_pcncc.x = pos_x;
        pos_pcncc.y =  (bsy - ((sy_ori-1) - pos_y)) - bty + (bbsy-1);
        pos_pcncc.z = pos_z;

        Y_IMG_REAL pOut(p_f_sub, szPad);
        Y_IMG_REAL pIn(p_f_tar, szPad);

        YImg<REAL, V3DLONG, Y_IMG_REAL, Y_IMG_REAL> tmp;
        tmp.fftnccp3D(pOut, pIn, szSub, szTar, even_odd, fftw_in_place, scale, &pos_pcncc);

        pos_score = pos_pcncc.value;

        //qDebug() << " bb normalized cross correlation ..." << pos_score;

        pos_x = pos_pcncc.x; pos_y = (sy_ori-1) - (bsy - (bty + pos_pcncc.y - (bbsy-1))); pos_z = pos_pcncc.z; //

        //de-alloc
        if(p_f_sub) {delete []p_f_sub; p_f_sub=0;}
        if(p_f_tar) {delete []p_f_tar; p_f_tar=0;}

    }
    else if(plane_xy == true)
    {
        // finding rich information plane from sub aV3DLONG z
        V3DLONG info_count=0, zpln, max_info=0;

        V3DLONG weights = bbsz*0.15;
        weights /= 2;
        V3DLONG start_z=bsz+weights+nbbz, end_z=esz-weights-nbbz;
        for(V3DLONG k=start_z; k<=end_z; k++) //
        {
            info_count = 0;

            V3DLONG offset_o_k = k*sx*sy;
            for(V3DLONG j=bsy; j<=esy; j++)
            {
                V3DLONG offset_o_j = j*sx;
                for(V3DLONG i=bsx; i<=esx; i++)
                {
                    V3DLONG idx_o = offset_o_k + offset_o_j + i;

                    if( p1dImg1[idx_o] > meanv)
                        info_count++;
                }
            }

            if(info_count > max_info)
            {
                max_info = info_count;
                zpln = k;
            }

        }

        // extraction
        V3DLONG b_bsz = zpln - weights/2;

        if(b_bsz>bsz)
            bsz = b_bsz;

        V3DLONG e_esz = zpln + weights/2;

        if(e_esz<esz)
            esz = e_esz;

        bbsz = esz-bsz+1; // dims

        // crop corresponding thick planes from tar image
        V3DLONG b_btz = bsz + offset[2] - nbbz;

        if(b_btz>btz)
            btz = b_btz;

        V3DLONG e_etz = esz + offset[2] + nbbz;

        if(e_etz<etz)
            etz = e_etz;

        bbtz = etz-btz+1;

        //qDebug() << " updated boundary ... "<< bsx << bsy << bsz << esx << esy << esz << btx << bty << btz << etx << ety << etz;
        //qDebug() << " updated dims ... "<< bbsx << bbsy << bbsz << bbtx << bbty << bbtz;

        V3DLONG pagesz_bb_sub = bbsx*bbsy*bbsz;
        V3DLONG pagesz_bb_tar = bbtx*bbty*bbtz;

        // extract one plane from sub
        Tdata* p_sub = new Tdata [pagesz_bb_sub];
        if (!p_sub)
        {
            printf("Fail to allocate memory.\n");
            return -1;
        }
        else
        {
            for(V3DLONG k=bsz; k<=esz; k++)
            {
                V3DLONG offset_k = (k-bsz)*bbsx*bbsy;
                V3DLONG offset_o_k = k*sx*sy;
                for(V3DLONG j=bsy; j<=esy; j++)
                {
                    V3DLONG offset_j = offset_k + (j-bsy)*bbsx;
                    V3DLONG offset_o_j = offset_o_k + j*sx;
                    for(V3DLONG i=bsx; i<=esx; i++)
                    {
                        V3DLONG idx = offset_j + (i-bsx);
                        V3DLONG idx_o = offset_o_j + i;

                        p_sub[idx] = p1dImg1[idx_o];
                    }
                }
            }
        }

        Tdata* p_tar = new Tdata [pagesz_bb_tar];
        if (!p_tar)
        {
            printf("Fail to allocate memory.\n");
            return -1;
        }

        //REAL max_response = 0;
        // search the best match plane from tar
        for(V3DLONG k=btz; k<=etz; k++)
        {
            V3DLONG offset_k = (k-btz)*bbtx*bbty;
            V3DLONG offset_o_k = k*tx*ty;
            for(V3DLONG j=bty; j<=ety; j++)
            {
                V3DLONG offset_j = offset_k + (j-bty)*bbtx;
                V3DLONG offset_o_j = offset_o_k + j*tx;
                for(V3DLONG i=btx; i<=etx; i++)
                {
                    V3DLONG idx = offset_j + (i-btx);
                    V3DLONG idx_o = offset_o_j + i;

                    p_tar[idx] = p1dImg2[idx_o];
                }
            }
        }

        //
        sx = bbsx; sy = bbsy; sz = bbsz;
        tx = bbtx; ty = bbty; tz = bbtz;

        V3DLONG sx_pad = sx+tx-1, sy_pad = sy+ty-1, sz_pad = sz+tz-1;

        V3DLONG even_odd = sx_pad%2; // 0 for even 1 for odd

        bool fftw_in_place = true;

        if(fftw_in_place)
            sx_pad += (2-even_odd); //2*(sx_pad/2 + 1); // fftw_in_place

        V3DLONG len_pad = sx_pad*sy_pad*sz_pad;

        szPad[0] = sx_pad;
        szPad[1] = sy_pad;
        szPad[2] = sz_pad;

        szTar[0] = tx;
        szTar[1] = ty;
        szTar[2] = tz;

        szSub[0] = sx;
        szSub[1] = sy;
        szSub[2] = sz;

        REAL* p_f_sub = new REAL [len_pad];
        if (!p_f_sub)
        {
            printf("Fail to allocate memory.\n");
            return -1;
        }
        else
        {
            // init
            memset(p_f_sub, 0, sizeof(REAL)*len_pad);

            //padding zeros for sub image
            Y_IMG_REAL pOut(p_f_sub, szPad);
            Y_IMG_DATATYPE pIn(p_sub, szSub);

            YImg<REAL, V3DLONG, Y_IMG_REAL, Y_IMG_DATATYPE> tmp;
            tmp.padding(pOut, pIn, true, fftw_in_place, even_odd, 3);

        }

        REAL* p_f_tar = new REAL [len_pad];
        if (!p_f_tar)
        {
            printf("Fail to allocate memory.\n");
            return -1;
        }
        else
        {
            // init
            memset(p_f_tar, 0, sizeof(REAL)*len_pad);

            //padding zeros for tar image
            Y_IMG_REAL pOut(p_f_tar, szPad);
            Y_IMG_DATATYPE pIn(p_tar, szTar);

            YImg<REAL, V3DLONG, Y_IMG_REAL, Y_IMG_DATATYPE> tmp;
            tmp.padding(pOut, pIn, false, fftw_in_place, even_odd, 3);

        }

        //de-alloc
        if(p_sub) {delete []p_sub; p_sub=0;}
        if(p_tar) {delete []p_tar; p_tar=0;}


        // fft-ncc
        PEAKS pos_pcncc;

        pos_pcncc.x = pos_x;
        pos_pcncc.y = pos_y;
        pos_pcncc.z = (bsz - ((sz_ori-1) - pos_z)) - btz + (bbsz-1);

        Y_IMG_REAL pOut(p_f_sub, szPad);
        Y_IMG_REAL pIn(p_f_tar, szPad);

        YImg<REAL, V3DLONG, Y_IMG_REAL, Y_IMG_REAL> tmp;
        tmp.fftnccp3D(pOut, pIn, szSub, szTar, even_odd, fftw_in_place, scale, &pos_pcncc);

        pos_score = pos_pcncc.value;

        //qDebug() << " bb normalized cross correlation ..." << pos_score;

        pos_x = pos_pcncc.x; pos_y = pos_pcncc.y; pos_z = (sz_ori-1) - (bsz - (btz + pos_pcncc.z - (bbsz-1))); //

        //de-alloc
        if(p_f_sub) {delete []p_f_sub; p_f_sub=0;}
        if(p_f_tar) {delete []p_f_tar; p_f_tar=0;}
    }

    //de-alloc
    if(szPad) {delete []szPad; szPad=0;}
    if(szSub) {delete []szSub; szSub=0;}
    if(szTar) {delete []szTar; szTar=0;}
    if(scale) {delete []scale; scale=0;}

    // outputs
    outputoffsets[0] = pos_x - sx_ori + 1;
    outputoffsets[1] = pos_y - sy_ori + 1;
    outputoffsets[2] = pos_z - sz_ori + 1;

    coef = pos_score;

    //
    return true;
}

//
template <class Tdata, class Tidx, class Y_IMG_DATATYPE>
bool transformations_detection(Tdata *p1dImg1, Tdata *p1dImg2, Tidx *szImg, int &transtype)
{
    /// transtype : 0 rigid; 1 non-rigid

    // assume the dimensions same
    Tidx sx = szImg[0];
    Tidx sy = szImg[1];
    Tidx sz = szImg[2];
    Tidx sc = szImg[3];

    // cubify
    vector<TransStatistics> transList;

    // block
    Tidx cubesz_x = sx/CUBEFACTOR;
    Tidx cubesz_y = sy/CUBEFACTOR;
    Tidx cubesz_z = sz/CUBEFACTOR;

    Tidx bx=cubesz_x;
    Tidx by=cubesz_y;
    Tidx bz=cubesz_z;
    Tidx bc=sc; // =1 only the reference channel

    Tidx szCube[4];
    szCube[0] = bx;
    szCube[1] = by;
    szCube[2] = bz;
    szCube[3] = bc;

    Tidx offsetc = sx*sy*sz;
    Tidx offsetz = sx*sy;
    Tidx offsety = sx;

    Tidx offsetc_block = bx*by*bz;
    Tidx offsetz_block = bx*by;
    Tidx offsety_block = bx;

    //
    Tidx xn = sx/bx;
    Tidx yn = sy/by;
    Tidx zn = sz/bz;

    if( 2*(sx-xn*bx) > bx ) xn++;
    if( 2*(sy-yn*by) > by ) yn++;
    if( 2*(sz-zn*bz) > bz ) zn++;

    //
    Tdata *pCubeS=NULL, *pCubeT=NULL;
    Tidx *offset=NULL;
    for (Tidx k=0; k<zn; k++)
    {
        for (Tidx j=0; j<yn; j++)
        {
            for (Tidx i=0; i<xn; i++)
            {
                // init
                TransStatistics tranSta;

                try
                {
                    y_new<Tdata, Tidx>(pCubeS, offsetc_block);
                    y_new<Tdata, Tidx>(pCubeT, offsetc_block);

                    memset(pCubeS, 0, sizeof(Tdata)*offsetc_block);
                    memset(pCubeT, 0, sizeof(Tdata)*offsetc_block);

                    y_new<Tidx, Tidx>(offset, 3);
                } catch (...) {
                    cout<<"Fail to allocate memory for block!"<<endl;
                    return false;
                }

                //
                Tidx start_x = i*bx;
                Tidx end_x = start_x + bx;

                if(end_x>sx) end_x = sx;

                Tidx start_y = j*by;
                Tidx end_y = start_y + by;

                if(end_y>sy) end_y = sy;

                Tidx start_z = k*bz;
                Tidx end_z = start_z + bz;

                if(end_z>sz) end_z = sz;

                //
                if(end_x<start_x || end_y<start_y || end_z<start_z) continue;

                //
                for(Tidx kk=start_z; kk<end_z; kk++)
                {
                    Tidx offset_z_ori = kk*offsetz;
                    Tidx offset_z_blk = (kk-start_z)*offsetz_block;

                    for(Tidx jj=start_y; jj<end_y; jj++)
                    {
                        Tidx offset_y_ori = offset_z_ori + jj*offsety;
                        Tidx offset_y_blk = offset_z_blk + (jj-start_y)*offsety_block;

                        for(Tidx ii=start_x; ii<end_x; ii++)
                        {
                            Tidx offset_x_ori = offset_y_ori + ii;
                            Tidx offset_x_blk = offset_y_blk + ii-start_x;

                            pCubeS[offset_x_blk] = p1dImg1[offset_x_ori];
                            pCubeT[offset_x_blk] = p1dImg2[offset_x_ori];
                        }
                    }
                }

                //
                REAL coef;
                ref_aligning<Tdata, Y_IMG_DATATYPE>(pCubeS, pCubeT, szCube, offset, coef);

                tranSta.dx = offset[0];
                tranSta.dy = offset[1];
                tranSta.dz = offset[2];
                tranSta.coef = coef;

                transList.push_back(tranSta);

                // de-alloc
                y_del<Tdata>(pCubeS);
                y_del<Tdata>(pCubeT);
                y_del<Tidx>(offset);
            }
        }
    }

    // likelihood func f(stdx, stdy, stdz, 1-meancoef)
    REAL mean_x=0, mean_y=0, mean_z=0, mean_coef=0;
    REAL std_x=0, std_y=0, std_z=0, std_coef=0;
    Tidx szSta = transList.size();

    for(Tidx i=0; i<szSta; i++)
    {
        mean_x += transList.at(i).dx;
        mean_y += transList.at(i).dy;
        mean_z += transList.at(i).dz;
        mean_coef += transList.at(i).coef;
    }
    mean_x /= szSta;
    mean_y /= szSta;
    mean_z /= szSta;
    mean_coef /= szSta;

    for(Tidx i=0; i<szSta; i++)
    {
        std_x += (transList.at(i).dx - mean_x)*(transList.at(i).dx - mean_x);
        std_y += (transList.at(i).dy - mean_y)*(transList.at(i).dy - mean_y);
        std_z += (transList.at(i).dz - mean_z)*(transList.at(i).dz - mean_z);
        std_coef += (transList.at(i).coef - mean_coef)*(transList.at(i).coef - mean_coef);
    }
    szSta--;
    std_x /= szSta;
    std_y /= szSta;
    std_z /= szSta;
    std_coef /= szSta;

    REAL f = 0; // likelihood

    f += std_x*std_x;
    f += std_y*std_y;
    f += std_z*std_z;
    f += (1-mean_coef)*(1-mean_coef);

    if(f<1.0)
        transtype = 0;
    else
        transtype = 1;

    //
    return true;
}

//
bool stitch_paired_images_with_refchan(Image4DSimple &p4DImage1, V3DLONG ref1, Image4DSimple &p4DImage2, V3DLONG ref2, bool b_transdet, int &transtype)
{
    // stitching to obtain the translation offsets
    // image 1 will be translated to image 2

    unsigned char* p1dImg1 = p4DImage1.getRawData();
    unsigned char* p1dImg2 = p4DImage2.getRawData();

    V3DLONG *szImg = new V3DLONG [4];

    szImg[0] = p4DImage1.getXDim();
    szImg[1] = p4DImage1.getYDim();
    szImg[2] = p4DImage1.getZDim();
    szImg[3] = p4DImage1.getCDim();

    V3DLONG datatype_img = p4DImage1.getUnitBytes();

    V3DLONG pagesz = szImg[0]*szImg[1]*szImg[2];
    V3DLONG totalplxs = datatype_img*pagesz*szImg[3];

    V3DLONG offset1 = ref1*pagesz;
    V3DLONG offset2 = ref2*pagesz;

    V3DLONG *offset=NULL;
    unsigned char* pTemImg = NULL;

    try {
        offset = new V3DLONG [3];
        pTemImg = new unsigned char [totalplxs];

        memset(pTemImg, 0, totalplxs); // init by 0
    }
    catch (...) {
        cout<<"Fail to allocate memory!"<<endl;
        return false;
    }

    // align
    REAL coef;
    if(datatype_img == V3D_UINT8)
    {
        if(b_transdet)
        {
            if(!transformations_detection<unsigned char, V3DLONG, Y_IMG_UINT8>((unsigned char*)(p1dImg1)+offset1, (unsigned char*)(p1dImg2)+offset2, szImg, transtype))
            {
                cout<<"Fail to detect transformations."<<endl;
                return false;
            }

            return true;
        }

        if(!ref_aligning<unsigned char, Y_IMG_UINT8>((unsigned char*)p1dImg1+offset1, (unsigned char*)p1dImg2+offset2, szImg, offset, coef))
        {
            cout<<"Fail to align referece color channels."<<endl;
            return false;
        }
    }
    else if(datatype_img == V3D_UINT16)
    {
        if(b_transdet)
        {
            if(!transformations_detection<unsigned short, V3DLONG, Y_IMG_UINT16>((unsigned short*)(p1dImg1)+offset1, (unsigned short*)(p1dImg2)+offset2, szImg, transtype))
            {
                cout<<"Fail to detect transformations."<<endl;
                return false;
            }

            return true;
        }

        if(!ref_aligning<unsigned short, Y_IMG_UINT16>((unsigned short*)(p1dImg1)+offset1, (unsigned short*)(p1dImg2)+offset2, szImg, offset, coef))
        {
            cout<<"Fail to align referece color channels."<<endl;
            return false;
        }
    }
    else if(datatype_img == V3D_FLOAT32)
    {
        if(b_transdet)
        {
            if(!transformations_detection<float, V3DLONG, Y_IMG_REAL>((float*)(p1dImg1)+offset1, (float*)(p1dImg2)+offset2, szImg, transtype))
            {
                cout<<"Fail to detect transformations."<<endl;
                return false;
            }

            return true;
        }

        if(!ref_aligning<REAL, Y_IMG_REAL>((REAL*)(p1dImg1)+offset1, (REAL*)(p1dImg2)+offset2, szImg, offset, coef))
        {
            cout<<"Fail to align referece color channels."<<endl;
            return false;
        }
    }
    else
    {
        cout<<"The blender only support UINT8, UINT16 and FLOAT32 datatype"<<endl;
        return false;
    }

    qDebug()<<"offset ..."<<offset[0]<<offset[1]<<offset[2];

    // adjust image 1
    V3DLONG offset_tx, offset_ty, offset_tz, offset_sx, offset_sy, offset_sz;

    //
    if(offset[0]<0)
    {
        offset_sx = 0; offset_tx = -offset[0];
    }
    else
    {
        offset_sx = offset[0]; offset_tx = 0;
    }
    if(offset[1]<0)
    {
        offset_sy = 0; offset_ty = -offset[1];
    }
    else
    {
        offset_sy = offset[1]; offset_ty = 0;
    }
    if(offset[2]<0)
    {
        offset_sz = 0; offset_tz = -offset[2];
    }
    else
    {
        offset_sz = offset[2]; offset_tz = 0;
    }

    //
    V3DLONG i_start, j_start, k_start;
    V3DLONG sz_start = offset_sz, sz_end = sz_start + szImg[2]; if(sz_start<0) k_start=0; else k_start=sz_start; if(sz_end>szImg[2]) sz_end=szImg[2];
    V3DLONG sy_start = offset_sy, sy_end = sy_start + szImg[1]; if(sy_start<0) j_start=0; else j_start=sy_start; if(sy_end>szImg[1]) sy_end=szImg[1];
    V3DLONG sx_start = offset_sx, sx_end = sx_start + szImg[0]; if(sx_start<0) i_start=0; else i_start=sx_start; if(sx_end>szImg[0]) sx_end=szImg[0];

    for(V3DLONG c=0; c<szImg[3]; c++)
    {
        V3DLONG offset_c = c*pagesz;
        for(V3DLONG k=k_start; k<sz_end; k++)
        {
            V3DLONG offset_k = offset_c + k*szImg[1]*szImg[0];

            V3DLONG kk = k - k_start + offset_tz;
            if(kk>=szImg[2]) break;

            V3DLONG offset_k_t = offset_c + kk*szImg[1]*szImg[0];

            for(V3DLONG j=j_start; j<sy_end; j++)
            {
                V3DLONG offset_j = offset_k + j*szImg[0];

                V3DLONG jj = j-j_start + offset_ty;
                if(jj>=szImg[1]) break;

                V3DLONG offset_j_t = offset_k_t + jj*szImg[0];

                for(V3DLONG i=i_start; i<sx_end; i++)
                {
                    V3DLONG ii = i - i_start + offset_tx;
                    if(ii>=szImg[0]) break;

                    V3DLONG idx = offset_j + i;
                    V3DLONG idx_t = offset_j_t + ii;

                    if(datatype_img == V3D_UINT8)
                    {
                        pTemImg[idx] = p1dImg1[idx_t];
                    }
                    else if(datatype_img == V3D_UINT16)
                    {
                        unsigned short *p = (unsigned short *)pTemImg;

                        p[idx] = ((unsigned short*)p1dImg1)[idx_t];
                    }
                    else if(datatype_img == V3D_FLOAT32)
                    {
                        float *p = (float *)pTemImg;

                        p[idx] = ((float*)p1dImg1)[idx_t];
                    }
                    else
                    {
                        cout<<"The blender only support UINT8, UINT16 and FLOAT32 datatype"<<endl;
                        return false;
                    }

                }
            }
        }
    }

    for(V3DLONG i=0; i<totalplxs; i++)
    {
        p1dImg1[i] = pTemImg[i];
    }

    // de-alloc
    if(pTemImg) {delete []pTemImg; pTemImg=NULL;}
    if(offset) {delete []offset; offset=NULL;}

    //
    return true;
}

// func mutual information for pair images with the same size
template <class Tdata>
double mi_computing(Tdata *pImg1, Tdata *pImg2, V3DLONG szImg, int datatype)
{
    size_t start_t = clock();

    // joint histogram
    double **jointHistogram = NULL;
    double *img1Hist=NULL;
    double *img2Hist=NULL;

    V3DLONG szHist;

    if(datatype==1) // 8-bit UINT8
    {
        szHist = 256;
    }
    else if(datatype==2) // 12-bit UINT16
    {
        szHist = 4096;
    }
    else
    {
        cout<<"Your data is not 8bit or 12bit and is currently not supported!"<<endl;
        return -1;
    }

    V3DLONG denomHist = szHist*szHist;

    try
    {
        jointHistogram = new double * [szHist];
        for(int i=0; i<szHist; i++)
        {
            jointHistogram[i] = new double [szHist];
            memset(jointHistogram[i], 0, sizeof(double)*szHist);
        }

        img1Hist = new double [szHist];
        img2Hist = new double [szHist];

        memset(img1Hist, 0, sizeof(double)*szHist);
        memset(img2Hist, 0, sizeof(double)*szHist);
    }
    catch(...)
    {
        qDebug()<<"Fail to allocate memory for joint histogram!";
        return -1;
    }

    //
    for(V3DLONG i=0; i<szImg; i++)
    {
        jointHistogram[ (V3DLONG)pImg1[i] ][ (V3DLONG)pImg2[i] ] ++;
    }

    double jointEntropy=0, img1Entropy=0, img2Entropy=0;

    // normalized joint histogram
    for(V3DLONG i=0; i<szHist; i++)
    {
        for(V3DLONG j=0; j<szHist; j++)
        {
            jointHistogram[i][j] /= (double)(denomHist);

            double val = jointHistogram[i][j]?jointHistogram[i][j]:1;

            jointEntropy += val * log2(val);
        }
    }

    // marginal histogram
    for(V3DLONG i=0; i<szHist; i++)
    {
        for(V3DLONG j=0; j<szHist; j++)
        {
            img1Hist[i] += jointHistogram[i][j];
            img2Hist[i] += jointHistogram[j][i];
        }
    }

    for(V3DLONG i=0; i<szHist; i++)
    {
        double val1 = img1Hist[i]?img1Hist[i]:1;
        double val2 = img2Hist[i]?img2Hist[i]:1;

        img1Entropy += val1 * log2(val1);
        img2Entropy += val2 * log2(val2);
    }

    // de-alloc
    if(jointHistogram) {
        for(int i=0; i<szHist; i++)
        {
            delete[] jointHistogram[i];
        }
        delete []jointHistogram; jointHistogram=NULL;
    }
    if(img1Hist) {delete []img1Hist; img1Hist=NULL;}
    if(img2Hist) {delete []img2Hist; img2Hist=NULL;}

    qDebug() << "time elapse for computing mutual information ... " <<clock()-start_t;

    // MI
    return (jointEntropy - img1Entropy - img2Entropy);

}

//plugin
const QString title = "Multiscan Image Blending";

// funcs
QStringList ImageBlendPlugin::funclist() const
{
    return QStringList() << tr("multiscanblend")
                         << tr("multiscanblend2")
                         << tr("transformdet")
                         << tr("extractchannels");
}

bool ImageBlendPlugin::dofunc(const QString & func_name, const V3DPluginArgList & input, V3DPluginArgList & output, V3DPluginCallback2 & v3d, QWidget * parent)
{
    //
    int ref_chans[2] = {-1, -1};
    bool b_override_channel = false; // User specified merge channels on command-line

    if (func_name == tr("multiscanblend"))
    {
        if(input.size()<1) return false; // no inputs

        vector<char*> * infilelist = (vector<char*> *)(input.at(0).p);
        vector<char*> * paralist;
        vector<char*> * outfilelist;

        if(infilelist->empty())
        {
            //print Help info
            printf("\nUsage: v3d -x blend_multiscanstacks.dylib -f multiscanblend -i <input_images> -o <output_image> -p \"#s <save_blending_result zero(false)/nonzero(true)> #k <b_morecolorstack_first nonzero(true)/zero(false)> #d <deformed nonzero(true)/zero(false) >\" \n");

            return true;
        }

        if(infilelist->size()<2)
        {
            printf("\nThe multiscan blending program needs two images as input!\n");

            return false;
        }

        qDebug()<<"input files ..."<<infilelist->at(0)<<infilelist->at(1);

        char * infile = infilelist->at(0); // input images
        char * paras = NULL; // parameters
        char * outfile = NULL; // outputs

        if(output.size()>0) { outfilelist = (vector<char*> *)(output.at(0).p); outfile = outfilelist->at(0);}  // specify output
        if(input.size()>1) { paralist = (vector<char*> *)(input.at(1).p); paras =  paralist->at(0);} // parameters

        bool b_saveimage = true; // save the blended image by default
        bool b_morecolorstack_first = false; // keep inputs order by default
        bool b_deformed = false; // rigid transformation by default

        // color mapping
        V3DLONG c10=-1, c11=-1, c12=-1;
        V3DLONG c20=-1, c21=-1;

        qDebug()<<"parameters ..."<<paras;

        if(paras)
        {
            int argc = 0;
            int len = strlen(paras);
            int posb[200];
            char * myparas = new char[len];
            strcpy(myparas, paras);
            for(int i = 0; i < len; i++)
            {
                if(i==0 && myparas[i] != ' ' && myparas[i] != '\t')
                {
                    posb[argc++] = i;
                }
                else if((myparas[i-1] == ' ' || myparas[i-1] == '\t') && (myparas[i] != ' ' && myparas[i] != '\t'))
                {
                    posb[argc++] = i;
                }
            }
            char ** argv = new char* [argc];
            for(int i = 0; i < argc; i++)
            {
                argv[i] = myparas + posb[i];
            }
            for(int i = 0; i < len; i++)
            {
                if(myparas[i]==' ' || myparas[i]=='\t')
                    myparas[i]='\0';
            }

            char* key;
            for(int i=0; i<argc; i++)
            {
                if(i+1 != argc) // check that we haven't finished parsing yet
                {
                    key = argv[i];

                    qDebug()<<">>key ..."<<key;

                    if (*key == '#')
                    {
                        while(*++key)
                        {
                            if (!strcmp(key, "s"))
                            {
                                b_saveimage = (atoi( argv[i+1] ))?true:false;
                                i++;
                            }
                            else if (!strcmp(key, "k"))
                            {
                                b_morecolorstack_first = (atoi( argv[i+1] )) ? true:false;
                                printf("Now set the b_morecolorstack_first = %s\n", (b_morecolorstack_first)? "TRUE" : "FALSE");
                                i++;
                            }
                            else if (!strcmp(key, "d"))
                            {
                                b_deformed = (atoi( argv[i+1] )) ? true:false;
                                printf("Now set the b_deformed = %s\n", (b_deformed)? "TRUE" : "FALSE");
                                i++;
                            }
                            else if (!strcmp(key, "r"))
                            {
                                ref_chans[0] = atoi( argv[++i] );
                                ref_chans[1] = atoi( argv[++i] );
                                b_override_channel = true;
                                printf("Using channels %d and %d as blending channels.\n", ref_chans[0], ref_chans[1]);
                                i++;
                            }
                            else
                            {
                                cout<<"parsing ..."<<key<<" "<<i<<" "<<"Unknown command. Type 'v3d -x plugin_name -f function_name' for usage"<<endl;
                                return false;
                            }

                        }
                    }
                    else
                    {
                        cout<<"parsing ..."<<key<<" "<<i<<" "<<"Unknown command. Type 'v3d -x plugin_name -f function_name' for usage"<<endl;
                        return false;
                    }

                }
            }
        }

        QString blendImageName;
        QString m_InputFileName(infile);
        m_InputFileName.chop(4);
        if(!outfile)
            blendImageName = m_InputFileName + "_blended.v3draw";
        else
            blendImageName = QString(outfile);

        if(QFileInfo(blendImageName).suffix().toUpper() != "V3DRAW")
        {
            blendImageName.append(".v3draw"); // force to save as .v3draw file
        }

        // image blending
        QString m_InputFileName1(infilelist->at(0));
        QString m_InputFileName2(infilelist->at(1));
        QString m_InputDeformed;

        if ( !QFile::exists(m_InputFileName1) || !QFile::exists(m_InputFileName2))
        {
            cout<<"Image does not exist!"<<endl;
            return false;
        }

        // if given deformed image
        if(b_deformed)
        {
            if(infilelist->size()<3)
            {
                cout<<"Invalid inputs given deformed transformation"<<endl;
                return false;
            }

            m_InputDeformed = QString(infilelist->at(2));
            if ( !QFile::exists(m_InputDeformed))
            {
                cout<<"Deformed Image does not exist!"<<endl;
                return false;
            }

        }

        // info reader and color configuration
        bool b_infoloaded = false;
        int *c1=NULL, *c2=NULL;

        // load images
        Image4DSimple p4DImage1, p4DImage2;

        V3DLONG *sz_img1 = 0;
        int datatype_img1 = 0;
        unsigned char* p1dImg1 = 0;

        if(QFileInfo(m_InputFileName1).suffix().toUpper().compare("LSM") == 0)
        {
            p4DImage1.loadImage(const_cast<char *>(m_InputFileName1.toStdString().c_str()), true); // Mylib

            b_morecolorstack_first = true;
        }
        else
        {
            p4DImage1.loadImage(const_cast<char *>(m_InputFileName1.toStdString().c_str()), false); // libtiff
        }

        p1dImg1 = p4DImage1.getRawData();

        sz_img1 = new V3DLONG [4];

        sz_img1[0] = p4DImage1.getXDim();
        sz_img1[1] = p4DImage1.getYDim();
        sz_img1[2] = p4DImage1.getZDim();
        sz_img1[3] = p4DImage1.getCDim();

        datatype_img1 = p4DImage1.getUnitBytes();

        V3DLONG *sz_img2 = 0;
        int datatype_img2 = 0;
        unsigned char* p1dImg2 = 0;

        if(QFileInfo(m_InputFileName2).suffix().toUpper().compare("LSM") == 0)
        {
            p4DImage2.loadImage(const_cast<char *>(m_InputFileName2.toStdString().c_str()), true); // Mylib

            b_morecolorstack_first = true;
        }
        else
        {
            p4DImage2.loadImage(const_cast<char *>(m_InputFileName2.toStdString().c_str()), false); // libtiff
        }

        p1dImg2 = p4DImage2.getRawData();

        sz_img2 = new V3DLONG [4];

        sz_img2[0] = p4DImage2.getXDim();
        sz_img2[1] = p4DImage2.getYDim();
        sz_img2[2] = p4DImage2.getZDim();
        sz_img2[3] = p4DImage2.getCDim();

        datatype_img2 = p4DImage2.getUnitBytes();

        // check dims datatype
        if(datatype_img1 != datatype_img2)
        {
            cout<<"Images are different data types! Do nothing!"<<endl;
            return false;
        }

        if(sz_img1[0] != sz_img2[0] || sz_img1[1] != sz_img2[1] || sz_img1[2] != sz_img2[2] ) // x, y, z
        {
            cout<<"Images are different dimensions! Do nothing!"<<endl;
            return false;
        }

        // swap inputs' order by choosing the input with more color channels as the first input
        if(b_morecolorstack_first && sz_img1[3]<sz_img2[3])
        {
            //
            qDebug()<<"original stack 1 "<<m_InputFileName1<<p1dImg1<<sz_img1[0]<<sz_img1[1]<<sz_img1[2]<<sz_img1[3]<<p4DImage1.getTotalUnitNumber();
            qDebug()<<"original stack 2 "<<m_InputFileName2<<p1dImg2<<sz_img2[0]<<sz_img2[1]<<sz_img2[2]<<sz_img2[3]<<p4DImage2.getTotalUnitNumber();

            //
            QString tmpName = m_InputFileName1;
            m_InputFileName1 = m_InputFileName2;
            m_InputFileName2 = tmpName;

            //
            unsigned char *p1 = NULL;
            unsigned char *p2 = NULL;

            try
            {
                V3DLONG totalplxs1 = p4DImage1.getTotalBytes();
                p1 = new unsigned char [totalplxs1];
                memcpy(p1, p4DImage1.getRawData(), totalplxs1);

                V3DLONG totalplxs2 = p4DImage2.getTotalBytes();
                p2 = new unsigned char [totalplxs2];
                memcpy(p2, p4DImage2.getRawData(), totalplxs2);
            }
            catch (...)
            {
                cout<<"Fail to allocate memory for swaping temporary pointers."<<endl;
                if (p1) {delete []p1; p1=0;}
                if (p2) {delete []p2; p2=0;}
                return false;
            }

            p4DImage1.setData(p2, sz_img2[0], sz_img2[1], sz_img2[2], sz_img2[3], (ImagePixelType)datatype_img2);
            p4DImage2.setData(p1, sz_img1[0], sz_img1[1], sz_img1[2], sz_img1[3], (ImagePixelType)datatype_img1);

            //
            p1dImg1 = p4DImage1.getRawData();
            sz_img1[3] = p4DImage1.getCDim();

            p1dImg2 = p4DImage2.getRawData();
            sz_img2[3] = p4DImage2.getCDim();

            qDebug()<<"switched stack 1 "<<m_InputFileName1<<p1dImg1<<sz_img1[0]<<sz_img1[1]<<sz_img1[2]<<sz_img1[3]<<p4DImage1.getTotalUnitNumber();
            qDebug()<<"switched stack 2 "<<m_InputFileName2<<p1dImg2<<sz_img2[0]<<sz_img2[1]<<sz_img2[2]<<sz_img2[3]<<p4DImage2.getTotalUnitNumber();
        }

        if( (QFileInfo(m_InputFileName1).suffix().toUpper().compare("LSM") == 0) && (QFileInfo(m_InputFileName2).suffix().toUpper().compare("LSM") == 0) )
        {
            //            char buf[512]; // read symbolic link
            //            int count = readlink(m_InputFileName1.toStdString().c_str(), buf, sizeof(buf));
            //            if (count >= 0) {
            //                buf[count] = '\0';
            //                printf("%s -> %s\n", m_InputFileName1.toStdString().c_str(), buf);
            //            }

            Y_LSMINFO<V3DLONG> lsminfo1(m_InputFileName1.toStdString());
            lsminfo1.loadHeader();

            c1 = new int [ sz_img1[3] ];
            for(int i=0; i<sz_img1[3]; i++)
            {
                c1[i] = getChannelNum(lsminfo1.colorchannels.at(i).R, lsminfo1.colorchannels.at(i).G, lsminfo1.colorchannels.at(i).B);
            }

            Y_LSMINFO<V3DLONG> lsminfo2(m_InputFileName2.toStdString());
            lsminfo2.loadHeader();

            c2 = new int [ sz_img2[3] ];
            for(int i=0; i<sz_img2[3]; i++)
            {
                c2[i] = getChannelNum(lsminfo2.colorchannels.at(i).R, lsminfo2.colorchannels.at(i).G, lsminfo2.colorchannels.at(i).B);
            }

            b_infoloaded = true;
        }

        //
        V3DLONG pagesz = sz_img1[0]*sz_img1[1]*sz_img1[2];

        // find reference : suppose reference color channels similar enough
        V3DLONG ref1=0, ref2=0, nullcolor1 = -1, nullcolor2 = -1;
        bool b_img1existNULL=false, b_img2existNULL=false;

        if(b_infoloaded)
        {
            for(int i=0; i<sz_img1[3]; i++)
            {
                if(c1[i]==3) {ref1 = i; break;}
            }

            for(int i=0; i<sz_img2[3]; i++)
            {
                if(c2[i]==3) {ref2 = i; break;}
            }
        }
        else
        {
            if (b_override_channel)
            {
                ref1 = ref_chans[0];
                ref2 = ref_chans[1];
            }
            else
            {
            // step 1: find null color channel
            for(V3DLONG c=0; c<sz_img1[3]; c++) // image 1
            {
                V3DLONG offset_c = c*pagesz;
                V3DLONG sumint1 = 0;
                for (V3DLONG k=0; k<sz_img1[2]; k++)
                {
                    V3DLONG offset_k = offset_c + k*sz_img1[0]*sz_img1[1];
                    for(V3DLONG j=0; j<sz_img1[1]; j++)
                    {
                        V3DLONG offset_j = offset_k + j*sz_img1[0];
                        for(V3DLONG i=0; i<sz_img1[0]; i++)
                        {
                            V3DLONG idx = offset_j + i;

                            if(datatype_img1 == V3D_UINT8)
                            {
                                sumint1 += p1dImg1[idx];
                            }
                            else if(datatype_img1 == V3D_UINT16)
                            {
                                sumint1 += ((unsigned short *)p1dImg1)[idx];
                            }
                            else if(datatype_img1 == V3D_FLOAT32)
                            {
                                sumint1 += ((float *)p1dImg1)[idx];
                            }
                            else
                            {
                                cout<<"Your image datatype is not supported!"<<endl;
                                return false;
                            }
                        }
                    }
                }

                qDebug()<<"sum ..."<<sumint1<<c;

                if(sumint1<EMPTY)
                {
                    b_img1existNULL = true;
                    nullcolor1 = c;
                }
            }

            for(V3DLONG c=0; c<sz_img2[3]; c++) // image 2
            {
                V3DLONG offset_c = c*pagesz;
                V3DLONG sumint2 = 0;
                for (V3DLONG k=0; k<sz_img1[2]; k++)
                {
                    V3DLONG offset_k = offset_c + k*sz_img1[0]*sz_img1[1];
                    for(V3DLONG j=0; j<sz_img1[1]; j++)
                    {
                        V3DLONG offset_j = offset_k + j*sz_img1[0];
                        for(V3DLONG i=0; i<sz_img1[0]; i++)
                        {
                            V3DLONG idx = offset_j + i;

                            if(datatype_img1 == V3D_UINT8)
                            {
                                sumint2 += p1dImg2[idx];
                            }
                            else if(datatype_img1 == V3D_UINT16)
                            {
                                sumint2 += ((unsigned short *)p1dImg2)[idx];
                            }
                            else if(datatype_img1 == V3D_FLOAT32)
                            {
                                sumint2 += ((float *)p1dImg2)[idx];
                            }
                            else
                            {
                                cout<<"Your image datatype is not supported!"<<endl;
                                return false;
                            }
                        }
                    }
                }

                qDebug()<<"sum ..."<<sumint2<<c;

                if(sumint2<EMPTY)
                {
                    b_img2existNULL = true;
                    nullcolor2 = c;
                }
            }

            // step 2: find ref color channel by compute MI
            double scoreMI = -1e10; // -INF
            for(V3DLONG c1=0; c1<sz_img1[3]; c1++)
            {
                if(b_img1existNULL)
                {
                     if ( c1 == nullcolor1 )
                        continue;
                }

                for(V3DLONG c2=0; c2<sz_img2[3]; c2++)
                {
                    if(b_img2existNULL)
                    {
                        if ( c2 == nullcolor2 )
                           continue;
                    }

                    if(datatype_img1 == V3D_UINT8)
                    {
                        unsigned char* pImg1Proxy = p1dImg1 + c1*pagesz;
                        unsigned char* pImg2Proxy = p1dImg2 + c2*pagesz;

                        double valMI = mi_computing< unsigned char >(
                            pImg1Proxy, pImg2Proxy, pagesz, 1 );

                        if(valMI>scoreMI)
                        {
                            scoreMI = valMI;

                            ref1 = c1;
                            ref2 = c2;
                        }
                    }
                    else if(datatype_img1 == V3D_UINT16)
                    {
                        unsigned short *pImg1Proxy =
                            ( (unsigned short *)p1dImg1 ) + c1 * pagesz;
                        unsigned short *pImg2Proxy =
                            ( (unsigned short *)p1dImg2 ) + c2 * pagesz;

                        double valMI = mi_computing< unsigned short >(
                            pImg1Proxy, pImg2Proxy, pagesz, 2 );

                        qDebug()<<"mi ..."<<valMI<<c1<<c2;

                        if(valMI>scoreMI)
                        {
                            scoreMI = valMI;

                            ref1 = c1;
                            ref2 = c2;
                        }
                    }
                    else if(datatype_img1 == V3D_FLOAT32)
                    {
                        printf(
                            "Currently this program dose not support FLOAT32.\n" ); // temporary
                        return false;
                    }
                    else
                    {
                        printf( "Currently this program only support UINT8, UINT16, and "
                                "FLOAT32 datatype.\n" );
                        return false;
                    }
                }
            }
        }
        }
        qDebug()<<"ref ..."<<ref1<<ref2<<"null color ..."<<b_img1existNULL<<nullcolor1<<b_img2existNULL<<nullcolor2;

        // if given deformed image
        if(b_deformed)
        {
            Image4DSimple p4DImgDeformed;

            V3DLONG *sz_deformed = 0;
            int datatype_deformed = 0;
            unsigned char* p1dDeformed = 0;

            if(QFileInfo(m_InputDeformed).suffix().toUpper().compare("LSM") == 0)
            {
                p4DImgDeformed.loadImage(const_cast<char *>(m_InputDeformed.toStdString().c_str()), true); // Mylib
            }
            else
            {
                p4DImgDeformed.loadImage(const_cast<char *>(m_InputDeformed.toStdString().c_str()), false); // libtiff
            }

            p1dDeformed = p4DImgDeformed.getRawData();

            sz_deformed = new V3DLONG [4];

            sz_deformed[0] = p4DImgDeformed.getXDim();
            sz_deformed[1] = p4DImgDeformed.getYDim();
            sz_deformed[2] = p4DImgDeformed.getZDim();
            sz_deformed[3] = p4DImgDeformed.getCDim();

            datatype_deformed = p4DImgDeformed.getUnitBytes();

            // convert data type from float to UINT8 or UINT16
            void *p=NULL;
            if(datatype_deformed!=datatype_img1)
            {
                // enhance contrast

                // convert datatype
                REAL max = -INF;
                REAL min = INF;

                for(V3DLONG i=0; i<pagesz; i++)
                {
                    REAL val = ((REAL *)p1dDeformed)[i];
                    if(val>max) max = val;
                    if(val<min) min = val;
                }
                max -= min;

                if(datatype_img1==V3D_UINT8)
                {
                    try
                    {
                        p = new char [pagesz];

                        unsigned char *pU8 = (unsigned char *)p;
                        for(V3DLONG i=0; i<pagesz; i++)
                        {
                            pU8[i] = 255*(((REAL *)p1dDeformed)[i] - min ) / max;
                        }
                    }
                    catch(...)
                    {
                        cout<<"Fail to allocate memory"<<endl;
                        return false;
                    }
                }
                else if(datatype_img1==V3D_UINT16)
                {
                    try
                    {
                        p = new unsigned short [pagesz];

                        unsigned short* pU16 = (unsigned short *)p;
                        for(V3DLONG i=0; i<pagesz; i++)
                        {
                            pU16[i] = 4096*(((REAL *)p1dDeformed)[i] - min ) / max;
                        }
                    }
                    catch(...)
                    {
                        cout<<"Fail to allocate memory"<<endl;
                        return false;
                    }
                }
                else
                {
                    cout<<"Invalid datatype"<<endl;
                    return false;
                }
            }

            // blend
            V3DLONG colordim = sz_img1[3]+sz_img2[3]-1;

            if(b_img1existNULL) colordim--;
            if(b_img2existNULL) colordim--;

            V3DLONG sz_blend[4];
            sz_blend[0] = sz_img1[0]; sz_blend[1] = sz_img1[1]; sz_blend[2] = sz_img1[2]; sz_blend[3] = colordim;

            V3DLONG totalplxs = colordim * pagesz;
            if(datatype_img1 == V3D_UINT8)
            {
                //
                unsigned char* data1d = NULL;
                try
                {
                    data1d = new unsigned char [totalplxs];

                    memset(data1d, 0, sizeof(unsigned char)*totalplxs);
                }
                catch(...)
                {
                    printf("Fail to allocate memory.\n");
                    return false;
                }

                //
                if(b_infoloaded)
                {
                    for(V3DLONG c=0; c<colordim-1; c++)
                    {
                        V3DLONG offset_c = c*pagesz;

                        V3DLONG offset_c1=0, offset_c2=0;
                        bool b_img1 = true;

                        for(V3DLONG ci=0; ci<sz_img1[3]; ci++)
                        {
                            if(c1[ci]==c) offset_c1 = ci*pagesz;
                        }

                        for(V3DLONG ci=0; ci<sz_img2[3]; ci++)
                        {
                            if(c2[ci]==c) {b_img1=false;}
                        }

                        for (V3DLONG k=0; k<sz_img1[2]; k++)
                        {
                            V3DLONG offset_k = offset_c + k*sz_img1[0]*sz_img1[1];

                            V3DLONG offset_k1 = offset_k - offset_c + offset_c1;
                            V3DLONG offset_k2 = offset_k - offset_c + offset_c2;

                            for(V3DLONG j=0; j<sz_img1[1]; j++)
                            {
                                V3DLONG offset_j = offset_k + j*sz_img1[0];

                                V3DLONG offset_j1 = offset_k1 + j*sz_img1[0];
                                V3DLONG offset_j2 = offset_k2 + j*sz_img1[0];

                                for(V3DLONG i=0; i<sz_img1[0]; i++)
                                {
                                    V3DLONG idx = offset_j + i;

                                    if (b_img1)
                                    {
                                        data1d[idx] = p1dImg1[offset_j1 + i];
                                    }
                                    else
                                    {
                                        data1d[idx] = ((unsigned char*)p)[offset_j2 + i];
                                    }
                                }
                            }
                        }
                    }
                }

                V3DLONG offset = (colordim-1)*pagesz;
                V3DLONG offset1 = ref1*pagesz;

                for(V3DLONG i=0; i<pagesz; i++)
                {
                    data1d[offset + i] = p1dImg1[offset1+i];
                }

                // step 5. cutting the blank plane introduced by blending
                unsigned char *pOutput = NULL;
                V3DLONG szOutput[4];

                img_cutting<unsigned char>( (unsigned char*)data1d, sz_blend, pOutput, szOutput);

                // de-alloc
                if(data1d) {delete []data1d; data1d=NULL;}

                // output
                if(b_saveimage)
                {
                    //save
                    if (saveImage(blendImageName.toStdString().c_str(), (const unsigned char *)pOutput, szOutput, 1)!=true)
                    {
                        fprintf(stderr, "Error happens in file writing. Exit. \n");
                        return false;
                    }

                    //de-alloc
                    if(pOutput) {delete []pOutput; pOutput=NULL;}
                }
                else
                {
                    V3DPluginArgItem arg;

                    arg.type = "data"; arg.p = (void *)(pOutput); output << arg;

                    V3DLONG metaImg[5]; // xyzc datatype
                    metaImg[0] = szOutput[0];
                    metaImg[1] = szOutput[1];
                    metaImg[2] = szOutput[2];
                    metaImg[3] = szOutput[3];
                    metaImg[4] = datatype_img1;

                    arg.type = "metaImage"; arg.p = (void *)(metaImg); output << arg;
                }

            }
            else if(datatype_img1 == V3D_UINT16)
            {
                //
                unsigned short* data1d = NULL;
                try
                {
                    data1d = new unsigned short [totalplxs];

                    memset(data1d, 0, sizeof(unsigned short)*totalplxs);
                }
                catch(...)
                {
                    printf("Fail to allocate memory.\n");
                    return -1;
                }

                if(b_infoloaded)
                {
                    for(V3DLONG c=0; c<colordim-1; c++)
                    {
                        V3DLONG offset_c = c*pagesz;

                        V3DLONG offset_c1=0, offset_c2=0;
                        bool b_img1 = true;

                        for(V3DLONG ci=0; ci<sz_img1[3]; ci++)
                        {
                            if(c1[ci]==c) offset_c1 = ci*pagesz;
                        }

                        for(V3DLONG ci=0; ci<sz_img2[3]; ci++)
                        {
                            if(c2[ci]==c) {b_img1=false;}
                        }

                        for (V3DLONG k=0; k<sz_img1[2]; k++)
                        {
                            V3DLONG offset_k = offset_c + k*sz_img1[0]*sz_img1[1];

                            V3DLONG offset_k1 = offset_k - offset_c + offset_c1;
                            V3DLONG offset_k2 = offset_k - offset_c + offset_c2;

                            for(V3DLONG j=0; j<sz_img1[1]; j++)
                            {
                                V3DLONG offset_j = offset_k + j*sz_img1[0];

                                V3DLONG offset_j1 = offset_k1 + j*sz_img1[0];
                                V3DLONG offset_j2 = offset_k2 + j*sz_img1[0];

                                for(V3DLONG i=0; i<sz_img1[0]; i++)
                                {
                                    V3DLONG idx = offset_j + i;

                                    if (b_img1)
                                    {
                                        data1d[idx] = ((unsigned short *)p1dImg1)[offset_j1 + i];
                                    }
                                    else
                                    {
                                        data1d[idx] = ((unsigned short *)p)[offset_j2 + i];
                                    }
                                }
                            }
                        }
                    }
                }

                V3DLONG offset = (colordim-1)*pagesz;
                V3DLONG offset1 = ref1*pagesz;

                for(V3DLONG i=0; i<pagesz; i++)
                {
                    data1d[offset + i] = ((unsigned short *)p1dImg1)[offset1+i];
                }

                // step 5. cutting the blank plane introduced by blending
                unsigned short *pOutput = NULL;
                V3DLONG szOutput[4];

                img_cutting<unsigned short>( (unsigned short*)data1d, sz_blend, pOutput, szOutput);

                // de-alloc
                if(data1d) {delete []data1d; data1d=NULL;}

                // output
                if(b_saveimage)
                {
                    //save
                    if (saveImage(blendImageName.toStdString().c_str(), (const unsigned char *)pOutput, szOutput, 2)!=true)
                    {
                        fprintf(stderr, "Error happens in file writing. Exit. \n");
                        return false;
                    }

                    //de-alloc
                    if(pOutput) {delete []pOutput; pOutput=NULL;}
                }
                else
                {
                    V3DPluginArgItem arg;

                    arg.type = "data"; arg.p = (void *)(pOutput); output << arg;

                    V3DLONG metaImg[5]; // xyzc datatype
                    metaImg[0] = szOutput[0];
                    metaImg[1] = szOutput[1];
                    metaImg[2] = szOutput[2];
                    metaImg[3] = szOutput[3];
                    metaImg[4] = datatype_img1;

                    arg.type = "metaImage"; arg.p = (void *)(metaImg); output << arg;
                }
            }
            else if(datatype_img1 == V3D_FLOAT32)
            {
                //
                float* data1d = NULL;
                try
                {
                    data1d = new float [totalplxs];

                    memset(data1d, 0, sizeof(float)*totalplxs);
                }
                catch(...)
                {
                    printf("Fail to allocate memory.\n");
                    return -1;
                }

                //
                if(b_infoloaded)
                {
                    for(V3DLONG c=0; c<colordim-1; c++)
                    {
                        V3DLONG offset_c = c*pagesz;

                        V3DLONG offset_c1=0, offset_c2=0;
                        bool b_img1 = true;

                        for(V3DLONG ci=0; ci<sz_img1[3]; ci++)
                        {
                            if(c1[ci]==c) offset_c1 = ci*pagesz;
                        }

                        for(V3DLONG ci=0; ci<sz_img2[3]; ci++)
                        {
                            if(c2[ci]==c) {b_img1=false;}
                        }

                        for (V3DLONG k=0; k<sz_img1[2]; k++)
                        {
                            V3DLONG offset_k = offset_c + k*sz_img1[0]*sz_img1[1];

                            V3DLONG offset_k1 = offset_k - offset_c + offset_c1;
                            V3DLONG offset_k2 = offset_k - offset_c + offset_c2;

                            for(V3DLONG j=0; j<sz_img1[1]; j++)
                            {
                                V3DLONG offset_j = offset_k + j*sz_img1[0];

                                V3DLONG offset_j1 = offset_k1 + j*sz_img1[0];
                                V3DLONG offset_j2 = offset_k2 + j*sz_img1[0];

                                for(V3DLONG i=0; i<sz_img1[0]; i++)
                                {
                                    V3DLONG idx = offset_j + i;

                                    if (b_img1)
                                    {
                                        data1d[idx] = ((float *)p1dImg1)[offset_j1 + i];
                                    }
                                    else
                                    {
                                        data1d[idx] = ((float *)p)[offset_j2 + i];
                                    }
                                }
                            }
                        }
                    }
                }

                V3DLONG offset = (colordim-1)*pagesz;
                V3DLONG offset1 = ref1*pagesz;

                for(V3DLONG i=0; i<pagesz; i++)
                {
                    data1d[offset + i] = ((float *)p1dImg1)[offset1+i];
                }

                // step 5. cutting the blank plane introduced by blending
                float *pOutput = NULL;
                V3DLONG szOutput[4];

                img_cutting<float>( (float*)data1d, sz_blend, pOutput, szOutput);

                // de-alloc
                if(data1d) {delete []data1d; data1d=NULL;}

                // output
                if(b_saveimage)
                {
                    //save
                    if (saveImage(blendImageName.toStdString().c_str(), (const unsigned char *)pOutput, szOutput, 4)!=true)
                    {
                        fprintf(stderr, "Error happens in file writing. Exit. \n");
                        return false;
                    }

                    //de-alloc
                    if(pOutput) {delete []pOutput; pOutput=NULL;}
                }
                else
                {
                    V3DPluginArgItem arg;

                    arg.type = "data"; arg.p = (void *)(pOutput); output << arg;

                    V3DLONG metaImg[5]; // xyzc datatype
                    metaImg[0] = szOutput[0];
                    metaImg[1] = szOutput[1];
                    metaImg[2] = szOutput[2];
                    metaImg[3] = szOutput[3];
                    metaImg[4] = datatype_img1;

                    arg.type = "metaImage"; arg.p = (void *)(metaImg); output << arg;
                }
            }
            else
            {
                printf("Currently this program only support UINT8, UINT16, FLOAT32 datatype.\n");
                return false;
            }

            // de-alloc
            y_del<V3DLONG>(sz_img1);
            y_del<V3DLONG>(sz_img2);
            y_del<V3DLONG>(sz_deformed);
            y_del<void>(p);

            //
            return true;
        }

        //step 3: need to run a simple stitching to figure out the displacement
        int transtype;
        if(!stitch_paired_images_with_refchan(p4DImage1, ref1, p4DImage2, ref2, false, transtype))
        {
            fprintf(stderr, "The stitching step fails and thus return.\n");
            return false;
        }

        // step 4: image blending
        // suppose image1 and image2 have a common reference
        // the blended image color dim = image1 color dim + image2 color dim - 1
        V3DLONG colordim = sz_img1[3]+sz_img2[3]-1;

        if(b_img1existNULL) colordim--;
        if(b_img2existNULL) colordim--;

        V3DLONG sz_blend[4];
        sz_blend[0] = sz_img1[0]; sz_blend[1] = sz_img1[1]; sz_blend[2] = sz_img1[2]; sz_blend[3] = colordim;

        V3DLONG totalplxs = colordim * pagesz;

        if(datatype_img1 == V3D_UINT8)
        {
            //
            unsigned char* data1d = NULL;
            try
            {
                data1d = new unsigned char [totalplxs];

                memset(data1d, 0, sizeof(unsigned char)*totalplxs);
            }
            catch(...)
            {
                printf("Fail to allocate memory.\n");
                return -1;
            }

            //
            if(b_infoloaded)
            {
                for(V3DLONG c=0; c<colordim-1; c++)
                {
                    V3DLONG offset_c = c*pagesz;

                    V3DLONG offset_c1, offset_c2;
                    bool b_img1 = true;

                    for(V3DLONG ci=0; ci<sz_img1[3]; ci++)
                    {
                        if(c1[ci]==c) offset_c1 = ci*pagesz;
                    }

                    for(V3DLONG ci=0; ci<sz_img2[3]; ci++)
                    {
                        if(c2[ci]==c) {offset_c2 = ci*pagesz; b_img1=false;}
                    }

                    for (V3DLONG k=0; k<sz_img1[2]; k++)
                    {
                        V3DLONG offset_k = offset_c + k*sz_img1[0]*sz_img1[1];

                        V3DLONG offset_k1 = offset_k - offset_c + offset_c1;
                        V3DLONG offset_k2 = offset_k - offset_c + offset_c2;

                        for(V3DLONG j=0; j<sz_img1[1]; j++)
                        {
                            V3DLONG offset_j = offset_k + j*sz_img1[0];

                            V3DLONG offset_j1 = offset_k1 + j*sz_img1[0];
                            V3DLONG offset_j2 = offset_k2 + j*sz_img1[0];

                            for(V3DLONG i=0; i<sz_img1[0]; i++)
                            {
                                V3DLONG idx = offset_j + i;

                                if (b_img1)
                                {
                                    data1d[idx] = p1dImg1[offset_j1 + i];
                                }
                                else
                                {
                                    data1d[idx] = p1dImg2[offset_j2 + i];
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                V3DLONG c1=0, c2=0;

                for(V3DLONG c=0; c<colordim-1; c++)
                {
                    V3DLONG offset_c = c*pagesz;

                    V3DLONG offset_c1, offset_c2;
                    bool b_img1;

                    if(c1<sz_img1[3])
                    {
                        b_img1 = true;

                        if(b_img1existNULL)
                        {
                            if(c1!=nullcolor1)
                            {
                                offset_c1 = c1*pagesz;
                            }
                            else
                            {
                                c1++;

                                if(c1<sz_img1[3])
                                    offset_c1 = c1*pagesz;
                                else
                                    b_img1 = false;
                            }
                        }

                        if(c1!=ref1)
                        {
                            offset_c1 = c1*pagesz;
                        }
                        else
                        {
                            c1++;

                            if(c1<sz_img1[3])
                                offset_c1 = c1*pagesz;
                            else
                                b_img1 = false;
                        }

                        qDebug()<<"color 1 ..."<<c1<<c;

                        c1++;
                    }
                    else
                    {
                        b_img1 = false;
                    }

                    if(!b_img1)
                    {
                        if(b_img2existNULL)
                        {
                            if(c2!=nullcolor2)
                            {
                                offset_c2 = c2*pagesz;
                            }
                            else
                            {
                                c2++;

                                if(c2<sz_img2[3])
                                    offset_c2 = c2*pagesz;
                                else
                                    continue;
                            }
                        }

                        if(c2!=ref2)
                        {
                            offset_c2 = c2*pagesz;
                        }
                        else
                        {
                            c2++;

                            if(c2<sz_img2[3])
                                offset_c2 = c2*pagesz;
                            else
                                continue;
                        }

                        qDebug()<<"color 2 ..."<<c2<<c;

                        c2++;
                    }

                    for (V3DLONG k=0; k<sz_img1[2]; k++)
                    {
                        V3DLONG offset_k = offset_c + k*sz_img1[0]*sz_img1[1];

                        V3DLONG offset_k1 = offset_k - offset_c + offset_c1;
                        V3DLONG offset_k2 = offset_k - offset_c + offset_c2;

                        for(V3DLONG j=0; j<sz_img1[1]; j++)
                        {
                            V3DLONG offset_j = offset_k + j*sz_img1[0];

                            V3DLONG offset_j1 = offset_k1 + j*sz_img1[0];
                            V3DLONG offset_j2 = offset_k2 + j*sz_img1[0];

                            for(V3DLONG i=0; i<sz_img1[0]; i++)
                            {
                                V3DLONG idx = offset_j + i;

                                if (b_img1)
                                {
                                    data1d[idx] = p1dImg1[offset_j1 + i];
                                }
                                else
                                {
                                    data1d[idx] = p1dImg2[offset_j2 + i];
                                }
                            }
                        }
                    }
                }
            }

            V3DLONG offset = (colordim-1)*pagesz;
            V3DLONG offset1 = ref1*pagesz;
            V3DLONG offset2 = ref2*pagesz;

            for(V3DLONG i=0; i<pagesz; i++)
            {
                data1d[offset + i] = 0.5*p1dImg1[offset1+i] + 0.5*p1dImg2[offset2+i];
            }

            // step 5. cutting the blank plane introduced by blending
            unsigned char *pOutput = NULL;
            V3DLONG szOutput[4];

            img_cutting<unsigned char>( (unsigned char*)data1d, sz_blend, pOutput, szOutput);

            // de-alloc
            if(data1d) {delete []data1d; data1d=NULL;}

            // output
            if(b_saveimage)
            {
                //save
                if (saveImage(blendImageName.toStdString().c_str(), (const unsigned char *)pOutput, szOutput, 1)!=true)
                {
                    fprintf(stderr, "Error happens in file writing. Exit. \n");
                    return false;
                }

                //de-alloc
                if(pOutput) {delete []pOutput; pOutput=NULL;}
            }
            else
            {
                V3DPluginArgItem arg;

                arg.type = "data"; arg.p = (void *)(pOutput); output << arg;

                V3DLONG metaImg[5]; // xyzc datatype
                metaImg[0] = szOutput[0];
                metaImg[1] = szOutput[1];
                metaImg[2] = szOutput[2];
                metaImg[3] = szOutput[3];
                metaImg[4] = datatype_img1;

                arg.type = "metaImage"; arg.p = (void *)(metaImg); output << arg;
            }

        }
        else if(datatype_img1 == V3D_UINT16)
        {
            //
            unsigned short* data1d = NULL;
            try
            {
                data1d = new unsigned short [totalplxs];

                memset(data1d, 0, sizeof(unsigned short)*totalplxs);
            }
            catch(...)
            {
                printf("Fail to allocate memory.\n");
                return -1;
            }

            if(b_infoloaded)
            {
                for(V3DLONG c=0; c<colordim-1; c++)
                {
                    V3DLONG offset_c = c*pagesz;

                    V3DLONG offset_c1, offset_c2;
                    bool b_img1 = true;

                    for(V3DLONG ci=0; ci<sz_img1[3]; ci++)
                    {
                        if(c1[ci]==c) offset_c1 = ci*pagesz;
                    }

                    for(V3DLONG ci=0; ci<sz_img2[3]; ci++)
                    {
                        if(c2[ci]==c) {offset_c2 = ci*pagesz; b_img1=false;}
                    }

                    for (V3DLONG k=0; k<sz_img1[2]; k++)
                    {
                        V3DLONG offset_k = offset_c + k*sz_img1[0]*sz_img1[1];

                        V3DLONG offset_k1 = offset_k - offset_c + offset_c1;
                        V3DLONG offset_k2 = offset_k - offset_c + offset_c2;

                        for(V3DLONG j=0; j<sz_img1[1]; j++)
                        {
                            V3DLONG offset_j = offset_k + j*sz_img1[0];

                            V3DLONG offset_j1 = offset_k1 + j*sz_img1[0];
                            V3DLONG offset_j2 = offset_k2 + j*sz_img1[0];

                            for(V3DLONG i=0; i<sz_img1[0]; i++)
                            {
                                V3DLONG idx = offset_j + i;

                                if (b_img1)
                                {
                                    data1d[idx] = ((unsigned short *)p1dImg1)[offset_j1 + i];
                                }
                                else
                                {
                                    data1d[idx] = ((unsigned short *)p1dImg2)[offset_j2 + i];
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                V3DLONG c1=0, c2=0;

                for(V3DLONG c=0; c<colordim-1; c++)
                {
                    V3DLONG offset_c = c*pagesz;

                    V3DLONG offset_c1, offset_c2;
                    bool b_img1;

                    if(c1<sz_img1[3])
                    {
                        b_img1 = true;

                        if(b_img1existNULL)
                        {
                            if(c1!=nullcolor1)
                            {
                                offset_c1 = c1*pagesz;
                            }
                            else
                            {
                                c1++;

                                if(c1<sz_img1[3])
                                    offset_c1 = c1*pagesz;
                                else
                                    b_img1 = false;
                            }
                        }

                        if(c1!=ref1)
                        {
                            offset_c1 = c1*pagesz;
                        }
                        else
                        {
                            c1++;

                            if(c1<sz_img1[3])
                                offset_c1 = c1*pagesz;
                            else
                                b_img1 = false;
                        }

                        qDebug()<<"color 1 ..."<<c1<<c;

                        c1++;
                    }
                    else
                    {
                        b_img1 = false;
                    }

                    if(!b_img1)
                    {
                        if(b_img2existNULL)
                        {
                            if(c2!=nullcolor2)
                            {
                                offset_c2 = c2*pagesz;
                            }
                            else
                            {
                                c2++;

                                if(c2<sz_img2[3])
                                    offset_c2 = c2*pagesz;
                                else
                                    continue;
                            }
                        }

                        if(c2!=ref2)
                        {
                            offset_c2 = c2*pagesz;
                        }
                        else
                        {
                            c2++;

                            if(c2<sz_img2[3])
                                offset_c2 = c2*pagesz;
                            else
                                continue;
                        }

                        qDebug()<<"color 2 ..."<<c2<<c;

                        c2++;
                    }

                    for (V3DLONG k=0; k<sz_img1[2]; k++)
                    {
                        V3DLONG offset_k = offset_c + k*sz_img1[0]*sz_img1[1];

                        V3DLONG offset_k1 = offset_k - offset_c + offset_c1;
                        V3DLONG offset_k2 = offset_k - offset_c + offset_c2;

                        for(V3DLONG j=0; j<sz_img1[1]; j++)
                        {
                            V3DLONG offset_j = offset_k + j*sz_img1[0];

                            V3DLONG offset_j1 = offset_k1 + j*sz_img1[0];
                            V3DLONG offset_j2 = offset_k2 + j*sz_img1[0];

                            for(V3DLONG i=0; i<sz_img1[0]; i++)
                            {
                                V3DLONG idx = offset_j + i;

                                if (b_img1)
                                {
                                    data1d[idx] = ((unsigned short *)p1dImg1)[offset_j1 + i];
                                }
                                else
                                {
                                    data1d[idx] = ((unsigned short *)p1dImg2)[offset_j2 + i];
                                }
                            }
                        }
                    }
                }
            }

            V3DLONG offset = (colordim-1)*pagesz;
            V3DLONG offset1 = ref1*pagesz;
            V3DLONG offset2 = ref2*pagesz;

            for(V3DLONG i=0; i<pagesz; i++)
            {
                data1d[offset + i] = 0.5*((unsigned short *)p1dImg1)[offset1+i] + 0.5*((unsigned short *)p1dImg2)[offset2+i];
            }

            // step 5. cutting the blank plane introduced by blending
            unsigned short *pOutput = NULL;
            V3DLONG szOutput[4];

            img_cutting<unsigned short>( (unsigned short*)data1d, sz_blend, pOutput, szOutput);

            qDebug()<<"output ..."<<pOutput;

            // de-alloc
            if(data1d) {delete []data1d; data1d=NULL;}

            // output
            if(b_saveimage)
            {
                //save
                if (saveImage(blendImageName.toStdString().c_str(), (const unsigned char *)pOutput, szOutput, 2)!=true)
                {
                    fprintf(stderr, "Error happens in file writing. Exit. \n");
                    return false;
                }

                //de-alloc
                if(pOutput) {delete []pOutput; pOutput=NULL;}
            }
            else
            {
                V3DPluginArgItem arg;

                arg.type = "data"; arg.p = (void *)(pOutput); output << arg;

                V3DLONG metaImg[5]; // xyzc datatype
                metaImg[0] = szOutput[0];
                metaImg[1] = szOutput[1];
                metaImg[2] = szOutput[2];
                metaImg[3] = szOutput[3];
                metaImg[4] = datatype_img1;

                arg.type = "metaImage"; arg.p = (void *)(metaImg); output << arg;
            }
        }
        else if(datatype_img1 == V3D_FLOAT32) //this section need further improvement, maybe discretization. by PHC, 110810
        {
            //
            float* data1d = NULL;
            try
            {
                data1d = new float [totalplxs];

                memset(data1d, 0, sizeof(float)*totalplxs);
            }
            catch(...)
            {
                printf("Fail to allocate memory.\n");
                return -1;
            }

            //
            if(b_infoloaded)
            {
                for(V3DLONG c=0; c<colordim-1; c++)
                {
                    V3DLONG offset_c = c*pagesz;

                    V3DLONG offset_c1, offset_c2;
                    bool b_img1 = true;

                    for(V3DLONG ci=0; ci<sz_img1[3]; ci++)
                    {
                        if(c1[ci]==c) offset_c1 = ci*pagesz;
                    }

                    for(V3DLONG ci=0; ci<sz_img2[3]; ci++)
                    {
                        if(c2[ci]==c) {offset_c2 = ci*pagesz; b_img1=false;}
                    }

                    for (V3DLONG k=0; k<sz_img1[2]; k++)
                    {
                        V3DLONG offset_k = offset_c + k*sz_img1[0]*sz_img1[1];

                        V3DLONG offset_k1 = offset_k - offset_c + offset_c1;
                        V3DLONG offset_k2 = offset_k - offset_c + offset_c2;

                        for(V3DLONG j=0; j<sz_img1[1]; j++)
                        {
                            V3DLONG offset_j = offset_k + j*sz_img1[0];

                            V3DLONG offset_j1 = offset_k1 + j*sz_img1[0];
                            V3DLONG offset_j2 = offset_k2 + j*sz_img1[0];

                            for(V3DLONG i=0; i<sz_img1[0]; i++)
                            {
                                V3DLONG idx = offset_j + i;

                                if (b_img1)
                                {
                                    data1d[idx] = ((float *)p1dImg1)[offset_j1 + i];
                                }
                                else
                                {
                                    data1d[idx] = ((float *)p1dImg2)[offset_j2 + i];
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                V3DLONG c1=0, c2=0;

                for(V3DLONG c=0; c<colordim-1; c++)
                {
                    V3DLONG offset_c = c*pagesz;

                    V3DLONG offset_c1, offset_c2;
                    bool b_img1;

                    if(c1<sz_img1[3])
                    {
                        b_img1 = true;

                        if(b_img1existNULL)
                        {
                            if(c1!=nullcolor1)
                            {
                                offset_c1 = c1*pagesz;
                            }
                            else
                            {
                                c1++;

                                if(c1<sz_img1[3])
                                    offset_c1 = c1*pagesz;
                                else
                                    b_img1 = false;
                            }
                        }

                        if(c1!=ref1)
                        {
                            offset_c1 = c1*pagesz;
                        }
                        else
                        {
                            c1++;

                            if(c1<sz_img1[3])
                                offset_c1 = c1*pagesz;
                            else
                                b_img1 = false;
                        }

                        qDebug()<<"color 1 ..."<<c1<<c;

                        c1++;
                    }
                    else
                    {
                        b_img1 = false;
                    }

                    if(!b_img1)
                    {
                        if(b_img2existNULL)
                        {
                            if(c2!=nullcolor2)
                            {
                                offset_c2 = c2*pagesz;
                            }
                            else
                            {
                                c2++;

                                if(c2<sz_img2[3])
                                    offset_c2 = c2*pagesz;
                                else
                                    continue;
                            }
                        }

                        if(c2!=ref2)
                        {
                            offset_c2 = c2*pagesz;
                        }
                        else
                        {
                            c2++;

                            if(c2<sz_img2[3])
                                offset_c2 = c2*pagesz;
                            else
                                continue;
                        }

                        qDebug()<<"color 2 ..."<<c2<<c;

                        c2++;
                    }

                    for (V3DLONG k=0; k<sz_img1[2]; k++)
                    {
                        V3DLONG offset_k = offset_c + k*sz_img1[0]*sz_img1[1];

                        V3DLONG offset_k1 = offset_k - offset_c + offset_c1;
                        V3DLONG offset_k2 = offset_k - offset_c + offset_c2;

                        for(V3DLONG j=0; j<sz_img1[1]; j++)
                        {
                            V3DLONG offset_j = offset_k + j*sz_img1[0];

                            V3DLONG offset_j1 = offset_k1 + j*sz_img1[0];
                            V3DLONG offset_j2 = offset_k2 + j*sz_img1[0];

                            for(V3DLONG i=0; i<sz_img1[0]; i++)
                            {
                                V3DLONG idx = offset_j + i;

                                if (b_img1)
                                {
                                    data1d[idx] = ((float *)p1dImg1)[offset_j1 + i];
                                }
                                else
                                {
                                    data1d[idx] = ((float *)p1dImg2)[offset_j2 + i];
                                }
                            }
                        }
                    }
                }
            }

            V3DLONG offset = (colordim-1)*pagesz;
            V3DLONG offset1 = ref1*pagesz;
            V3DLONG offset2 = ref2*pagesz;

            for(V3DLONG i=0; i<pagesz; i++)
            {
                data1d[offset + i] = 0.5*((float *)p1dImg1)[offset1+i] + 0.5*((float *)p1dImg2)[offset2+i];
            }

            // step 5. cutting the blank plane introduced by blending
            float *pOutput = NULL;
            V3DLONG szOutput[4];

            img_cutting<float>( (float*)data1d, sz_blend, pOutput, szOutput);

            // de-alloc
            if(data1d) {delete []data1d; data1d=NULL;}

            // output
            if(b_saveimage)
            {
                //save
                if (saveImage(blendImageName.toStdString().c_str(), (const unsigned char *)pOutput, szOutput, 4)!=true)
                {
                    fprintf(stderr, "Error happens in file writing. Exit. \n");
                    return false;
                }

                //de-alloc
                if(pOutput) {delete []pOutput; pOutput=NULL;}
            }
            else
            {
                V3DPluginArgItem arg;

                arg.type = "data"; arg.p = (void *)(pOutput); output << arg;

                V3DLONG metaImg[5]; // xyzc datatype
                metaImg[0] = szOutput[0];
                metaImg[1] = szOutput[1];
                metaImg[2] = szOutput[2];
                metaImg[3] = szOutput[3];
                metaImg[4] = datatype_img1;

                arg.type = "metaImage"; arg.p = (void *)(metaImg); output << arg;
            }
        }
        else
        {
            printf("Currently this program only support UINT8, UINT16, FLOAT32 datatype.\n");
            return false;
        }

        // de-alloc
        if(sz_img1) {delete []sz_img1; sz_img1=NULL;}
        if(sz_img2) {delete []sz_img2; sz_img2=NULL;}

    }
    else if (func_name == tr("multiscanblend2"))
    {
        if(input.size()<1) return false; // no inputs

        vector<char*> * infilelist = (vector<char*> *)(input.at(0).p);
        vector<char*> * paralist;
        vector<char*> * outfilelist;
        int ref_chans[2] = {-1, -1};

        if(infilelist->empty())
        {
            //print Help info
            printf("\nUsage: v3d -x blend_multiscanstacks -f multiscanblend2 -i <input_images> -o <output_image> -p \"#s <save_blending_result zero(false)/nonzero(true)> #d <deformed nonzero(true)/zero(false) >\" \n");

            return true;
        }

        if(infilelist->size()<2)
        {
            printf("\nThe multiscan blending program needs two images as input!\n");

            return false;
        }

        qDebug()<<"input files ..."<<infilelist->at(0)<<infilelist->at(1);

        char * infile = infilelist->at(0); // input images
        char * paras = NULL; // parameters
        char * outfile = NULL; // outputs

        if(output.size()>0) { outfilelist = (vector<char*> *)(output.at(0).p); outfile = outfilelist->at(0);}  // specify output
        if(input.size()>1) { paralist = (vector<char*> *)(input.at(1).p); paras =  paralist->at(0);} // parameters

        bool b_saveimage = true; // save the blended image by default
        bool b_deformed = false; // rigid transformation by default

        if(paras)
        {
            int argc = 0;
            int len = strlen(paras);
            int posb[200];
            char * myparas = new char[len];
            strcpy(myparas, paras);
            for(int i = 0; i < len; i++)
            {
                if(i==0 && myparas[i] != ' ' && myparas[i] != '\t')
                {
                    posb[argc++] = i;
                }
                else if((myparas[i-1] == ' ' || myparas[i-1] == '\t') && (myparas[i] != ' ' && myparas[i] != '\t'))
                {
                    posb[argc++] = i;
                }
            }
            char ** argv = new char* [argc];
            for(int i = 0; i < argc; i++)
            {
                argv[i] = myparas + posb[i];
            }
            for(int i = 0; i < len; i++)
            {
                if(myparas[i]==' ' || myparas[i]=='\t')
                    myparas[i]='\0';
            }

            char* key;
            for(int i=0; i<argc; i++)
            {
                if(i+1 != argc) // check that we haven't finished parsing yet
                {
                    key = argv[i];

                    qDebug()<<">>key ..."<<key;

                    if (*key == '#')
                    {
                        while(*++key)
                        {
                            if (!strcmp(key, "s"))
                            {
                                b_saveimage = (atoi( argv[i+1] ))?true:false;
                                i++;
                            }
                            else if (!strcmp(key, "d"))
                            {
                                b_deformed = (atoi( argv[i+1] )) ? true:false;
                                printf("Now set the b_deformed = %s\n", (b_deformed)? "TRUE" : "FALSE");
                                i++;
                            }
                            else if (!strcmp(key, "r"))
                            {
                                ref_chans[0] = atoi( argv[++i] );
                                ref_chans[1] = atoi( argv[++i] );
                                b_override_channel = true;
                                printf("Using channels %d and %d as blending channels.\n", ref_chans[0], ref_chans[1]);
                                i++;
                            }
                            else
                            {
                                cout<<"parsing ..."<<key<<" "<<i<<" "<<"Unknown command. Type 'v3d -x plugin_name -f function_name' for usage"<<endl;
                                return false;
                            }

                        }
                    }
                    else
                    {
                        cout<<"parsing ..."<<key<<" "<<i<<" "<<"Unknown command. Type 'v3d -x plugin_name -f function_name' for usage"<<endl;
                        return false;
                    }

                }
            }
        }

        QString blendImageName;
        QString m_InputFileName(infile);
        m_InputFileName.chop(4);
        if(!outfile)
            blendImageName = m_InputFileName + "_blended.v3draw";
        else
            blendImageName = QString(outfile);

        if(QFileInfo(blendImageName).suffix().toUpper() != "V3DRAW")
        {
            blendImageName.append(".v3draw"); // force to save as .v3draw file
        }

        // image blending
        QString m_InputFileName1(infilelist->at(0));
        QString m_InputFileName2(infilelist->at(1));
        QString m_InputDeformed;

        if ( !QFile::exists(m_InputFileName1) || !QFile::exists(m_InputFileName2))
        {
            cout<<"Image does not exist!"<<endl;
            return false;
        }

        // if given deformed image
        if(b_deformed)
        {
            if(infilelist->size()<3)
            {
                cout<<"Invalid inputs given deformed transformation"<<endl;
                return false;
            }

            m_InputDeformed = QString(infilelist->at(2));
            if ( !QFile::exists(m_InputDeformed))
            {
                cout<<"Deformed Image does not exist!"<<endl;
                return false;
            }

        }

        // load images
        Image4DSimple p4DImage1, p4DImage2;

        V3DLONG *sz_img1 = 0;
        int datatype_img1 = 0;
        unsigned char* p1dImg1 = 0;

        if(QFileInfo(m_InputFileName1).suffix().toUpper().compare("LSM") == 0)
        {
            p4DImage1.loadImage(const_cast<char *>(m_InputFileName1.toStdString().c_str()), true); // Mylib
        }
        else
        {
            p4DImage1.loadImage(const_cast<char *>(m_InputFileName1.toStdString().c_str()), false); // libtiff
        }

        p1dImg1 = p4DImage1.getRawData();

        sz_img1 = new V3DLONG [4];

        sz_img1[0] = p4DImage1.getXDim();
        sz_img1[1] = p4DImage1.getYDim();
        sz_img1[2] = p4DImage1.getZDim();
        sz_img1[3] = p4DImage1.getCDim();

        datatype_img1 = p4DImage1.getUnitBytes();

        V3DLONG *sz_img2 = 0;
        int datatype_img2 = 0;
        unsigned char* p1dImg2 = 0;

        if(QFileInfo(m_InputFileName2).suffix().toUpper().compare("LSM") == 0)
        {
            p4DImage2.loadImage(const_cast<char *>(m_InputFileName2.toStdString().c_str()), true); // Mylib
        }
        else
        {
            p4DImage2.loadImage(const_cast<char *>(m_InputFileName2.toStdString().c_str()), false); // libtiff
        }

        p1dImg2 = p4DImage2.getRawData();

        sz_img2 = new V3DLONG [4];

        sz_img2[0] = p4DImage2.getXDim();
        sz_img2[1] = p4DImage2.getYDim();
        sz_img2[2] = p4DImage2.getZDim();
        sz_img2[3] = p4DImage2.getCDim();

        datatype_img2 = p4DImage2.getUnitBytes();

        // check dims datatype
        if(datatype_img1 != datatype_img2)
        {
            cout<<"Images are different data types! Do nothing!"<<endl;
            return false;
        }

        if(sz_img1[0] != sz_img2[0] || sz_img1[1] != sz_img2[1] || sz_img1[2] != sz_img2[2] ) // x, y, z
        {
            cout<<"Images are different dimensions! Do nothing!"<<endl;
            return false;
        }

        //
        V3DLONG pagesz = sz_img1[0]*sz_img1[1]*sz_img1[2];

        //
        /// find reference : suppose reference color channels similar enough
        //
        V3DLONG ref1=0, ref2=0, nullcolor1 = -1, nullcolor2 = -1;
        bool b_img1existNULL=false, b_img2existNULL=false;

        if (b_override_channel)
        {
            ref1 = ref_chans[0];
            ref2 = ref_chans[1];
        }
        else
        {
        // step 1: find null color channel
        for(V3DLONG c=0; c<sz_img1[3]; c++) // image 1
        {
            V3DLONG offset_c = c*pagesz;
            V3DLONG sumint1 = 0;
            for (V3DLONG k=0; k<sz_img1[2]; k++)
            {
                V3DLONG offset_k = offset_c + k*sz_img1[0]*sz_img1[1];
                for(V3DLONG j=0; j<sz_img1[1]; j++)
                {
                    V3DLONG offset_j = offset_k + j*sz_img1[0];
                    for(V3DLONG i=0; i<sz_img1[0]; i++)
                    {
                        V3DLONG idx = offset_j + i;

                        if(datatype_img1 == V3D_UINT8)
                        {
                            sumint1 += p1dImg1[idx];
                        }
                        else if(datatype_img1 == V3D_UINT16)
                        {
                            sumint1 += ((unsigned short *)p1dImg1)[idx];
                        }
                        else if(datatype_img1 == V3D_FLOAT32)
                        {
                            sumint1 += ((float *)p1dImg1)[idx];
                        }
                        else
                        {
                            cout<<"Your image datatype is not supported!"<<endl;
                            return false;
                        }
                    }
                }
            }

            qDebug()<<"sum ..."<<sumint1<<c;

            if(sumint1<EMPTY)
            {
                b_img1existNULL = true;
                nullcolor1 = c;
            }
        }

        for(V3DLONG c=0; c<sz_img2[3]; c++) // image 2
        {
            V3DLONG offset_c = c*pagesz;
            V3DLONG sumint2 = 0;
            for (V3DLONG k=0; k<sz_img1[2]; k++)
            {
                V3DLONG offset_k = offset_c + k*sz_img1[0]*sz_img1[1];
                for(V3DLONG j=0; j<sz_img1[1]; j++)
                {
                    V3DLONG offset_j = offset_k + j*sz_img1[0];
                    for(V3DLONG i=0; i<sz_img1[0]; i++)
                    {
                        V3DLONG idx = offset_j + i;

                        if(datatype_img1 == V3D_UINT8)
                        {
                            sumint2 += p1dImg2[idx];
                        }
                        else if(datatype_img1 == V3D_UINT16)
                        {
                            sumint2 += ((unsigned short *)p1dImg2)[idx];
                        }
                        else if(datatype_img1 == V3D_FLOAT32)
                        {
                            sumint2 += ((float *)p1dImg2)[idx];
                        }
                        else
                        {
                            cout<<"Your image datatype is not supported!"<<endl;
                            return false;
                        }
                    }
                }
            }

            qDebug()<<"sum ..."<<sumint2<<c;

            if(sumint2<EMPTY)
            {
                b_img2existNULL = true;
                nullcolor2 = c;
            }
        }

        // step 2: find ref color channel by compute MI
        double scoreMI = -1e10; // -INF
        for(V3DLONG c1=0; c1<sz_img1[3]; c1++)
        {
            if(b_img1existNULL)
            {
                 if ( c1 == nullcolor1 )
                    continue;
            }

            for(V3DLONG c2=0; c2<sz_img2[3]; c2++)
            {
                if(b_img2existNULL)
                {
                    if ( c2 == nullcolor2 )
                       continue;
                }

                if(datatype_img1 == V3D_UINT8)
                {
                    unsigned char* pImg1Proxy = p1dImg1 + c1*pagesz;
                    unsigned char* pImg2Proxy = p1dImg2 + c2*pagesz;

                    double valMI = mi_computing< unsigned char >( pImg1Proxy, pImg2Proxy,
                                                                  pagesz, 1 );

                    if(valMI>scoreMI)
                    {
                        scoreMI = valMI;

                        ref1 = c1;
                        ref2 = c2;
                    }
                }
                else if(datatype_img1 == V3D_UINT16)
                {
                    unsigned short *pImg1Proxy =
                        ( (unsigned short *)p1dImg1 ) + c1 * pagesz;
                    unsigned short *pImg2Proxy =
                        ( (unsigned short *)p1dImg2 ) + c2 * pagesz;

                    double valMI = mi_computing< unsigned short >( pImg1Proxy, pImg2Proxy,
                                                                   pagesz, 2 );

                    if(valMI>scoreMI)
                    {
                        scoreMI = valMI;

                        ref1 = c1;
                        ref2 = c2;
                    }
                }
                else if(datatype_img1 == V3D_FLOAT32)
                {
                    printf(
                        "Currently this program dose not support FLOAT32.\n" ); // temporary
                    return false;
                }
                else
                {
                    printf( "Currently this program only support UINT8, UINT16, and "
                            "FLOAT32 datatype.\n" );
                    return false;
                }
            }
        }
        }

        qDebug()<<"ref ..."<<ref1<<ref2<<"null color ..."<<b_img1existNULL<<nullcolor1<<b_img2existNULL<<nullcolor2;

        // if given deformed image
        if(b_deformed)
        {
            Image4DSimple p4DImgDeformed;

            V3DLONG *sz_deformed = 0;
            int datatype_deformed = 0;
            unsigned char* p1dDeformed = 0;

            if(QFileInfo(m_InputDeformed).suffix().toUpper().compare("LSM") == 0)
            {
                p4DImgDeformed.loadImage(const_cast<char *>(m_InputDeformed.toStdString().c_str()), true); // Mylib
            }
            else
            {
                p4DImgDeformed.loadImage(const_cast<char *>(m_InputDeformed.toStdString().c_str()), false); // libtiff
            }

            p1dDeformed = p4DImgDeformed.getRawData();

            sz_deformed = new V3DLONG [4];

            sz_deformed[0] = p4DImgDeformed.getXDim();
            sz_deformed[1] = p4DImgDeformed.getYDim();
            sz_deformed[2] = p4DImgDeformed.getZDim();
            sz_deformed[3] = p4DImgDeformed.getCDim();

            datatype_deformed = p4DImgDeformed.getUnitBytes();

            // convert data type from float to UINT8 or UINT16
            void *p=NULL;
            if(datatype_deformed!=datatype_img1)
            {
                // enhance contrast

                // convert datatype
                REAL max = -INF;
                REAL min = INF;

                for(V3DLONG i=0; i<pagesz; i++)
                {
                    REAL val = ((REAL *)p1dDeformed)[i];
                    if(val>max) max = val;
                    if(val<min) min = val;
                }
                max -= min;

                if(datatype_img1==V3D_UINT8)
                {
                    try
                    {
                        p = new char [pagesz];

                        unsigned char *pU8 = (unsigned char *)p;
                        for(V3DLONG i=0; i<pagesz; i++)
                        {
                            pU8[i] = 255*(((REAL *)p1dDeformed)[i] - min ) / max;
                        }
                    }
                    catch(...)
                    {
                        cout<<"Fail to allocate memory"<<endl;
                        return false;
                    }
                }
                else if(datatype_img1==V3D_UINT16)
                {
                    try
                    {
                        p = new unsigned short [pagesz];

                        unsigned short* pU16 = (unsigned short *)p;
                        for(V3DLONG i=0; i<pagesz; i++)
                        {
                            pU16[i] = 4096*(((REAL *)p1dDeformed)[i] - min ) / max;
                        }
                    }
                    catch(...)
                    {
                        cout<<"Fail to allocate memory"<<endl;
                        return false;
                    }
                }
                else
                {
                    cout<<"Invalid datatype"<<endl;
                    return false;
                }
            }

            // blend
            V3DLONG colordim = sz_img1[3]+sz_img2[3]-1;

            if(b_img1existNULL) colordim--;
            if(b_img2existNULL) colordim--;

            V3DLONG sz_blend[4];
            sz_blend[0] = sz_img1[0]; sz_blend[1] = sz_img1[1]; sz_blend[2] = sz_img1[2]; sz_blend[3] = colordim;

            V3DLONG totalplxs = colordim * pagesz;
            if(datatype_img1 == V3D_UINT8)
            {
                //
                unsigned char* data1d = NULL;
                try
                {
                    data1d = new unsigned char [totalplxs];

                    memset(data1d, 0, sizeof(unsigned char)*totalplxs);
                }
                catch(...)
                {
                    printf("Fail to allocate memory.\n");
                    return false;
                }

                // construct image with sequence order of image1 and image2
                V3DLONG c1=0, c2=0;

                for(V3DLONG c=0; c<colordim-1; c++)
                {
                    V3DLONG offset_c = c*pagesz;

                    V3DLONG offset_c1, offset_c2;
                    bool b_img1;

                    if(c1<sz_img1[3])
                    {
                        b_img1 = true;

                        if(b_img1existNULL)
                        {
                            if(c1!=nullcolor1)
                            {
                                offset_c1 = c1*pagesz;
                            }
                            else
                            {
                                c1++;

                                if(c1<sz_img1[3])
                                    offset_c1 = c1*pagesz;
                                else
                                    b_img1 = false;
                            }
                        }

                        if(c1!=ref1)
                        {
                            offset_c1 = c1*pagesz;
                        }
                        else
                        {
                            c1++;

                            if(c1<sz_img1[3])
                                offset_c1 = c1*pagesz;
                            else
                                b_img1 = false;
                        }

                        qDebug()<<"color 1 ..."<<c1<<c;

                        c1++;
                    }
                    else
                    {
                        b_img1 = false;
                    }

                    if(!b_img1)
                    {
                        if(b_img2existNULL)
                        {
                            if(c2!=nullcolor2)
                            {
                                offset_c2 = c2*pagesz;
                            }
                            else
                            {
                                c2++;

                                if(c2<sz_img2[3])
                                    offset_c2 = c2*pagesz;
                                else
                                    continue;
                            }
                        }

                        if(c2!=ref2)
                        {
                            offset_c2 = c2*pagesz;
                        }
                        else
                        {
                            c2++;

                            if(c2<sz_img2[3])
                                offset_c2 = c2*pagesz;
                            else
                                continue;
                        }

                        qDebug()<<"color 2 ..."<<c2<<c;

                        c2++;
                    }

                    for (V3DLONG k=0; k<sz_img1[2]; k++)
                    {
                        V3DLONG offset_k = offset_c + k*sz_img1[0]*sz_img1[1];

                        V3DLONG offset_k1 = offset_k - offset_c + offset_c1;
                        V3DLONG offset_k2 = offset_k - offset_c + offset_c2;

                        for(V3DLONG j=0; j<sz_img1[1]; j++)
                        {
                            V3DLONG offset_j = offset_k + j*sz_img1[0];

                            V3DLONG offset_j1 = offset_k1 + j*sz_img1[0];
                            V3DLONG offset_j2 = offset_k2 + j*sz_img1[0];

                            for(V3DLONG i=0; i<sz_img1[0]; i++)
                            {
                                V3DLONG idx = offset_j + i;

                                if (b_img1)
                                {
                                    data1d[idx] = p1dImg1[offset_j1 + i];
                                }
                                else
                                {
                                    data1d[idx] = ((unsigned char*)p)[offset_j2 + i];
                                }
                            }
                        }
                    }
                }

                V3DLONG offset = (colordim-1)*pagesz;
                V3DLONG offset1 = ref1*pagesz;

                for(V3DLONG i=0; i<pagesz; i++)
                {
                    data1d[offset + i] = p1dImg1[offset1+i];
                }

                // step 5. cutting the blank plane introduced by blending
                unsigned char *pOutput = NULL;
                V3DLONG szOutput[4];

                img_cutting<unsigned char>( (unsigned char*)data1d, sz_blend, pOutput, szOutput);

                // de-alloc
                if(data1d) {delete []data1d; data1d=NULL;}

                // output
                if(b_saveimage)
                {
                    //save
                    if (saveImage(blendImageName.toStdString().c_str(), (const unsigned char *)pOutput, szOutput, 1)!=true)
                    {
                        fprintf(stderr, "Error happens in file writing. Exit. \n");
                        return false;
                    }

                    //de-alloc
                    if(pOutput) {delete []pOutput; pOutput=NULL;}
                }
                else
                {
                    V3DPluginArgItem arg;

                    arg.type = "data"; arg.p = (void *)(pOutput); output << arg;

                    V3DLONG metaImg[5]; // xyzc datatype
                    metaImg[0] = szOutput[0];
                    metaImg[1] = szOutput[1];
                    metaImg[2] = szOutput[2];
                    metaImg[3] = szOutput[3];
                    metaImg[4] = datatype_img1;

                    arg.type = "metaImage"; arg.p = (void *)(metaImg); output << arg;
                }

            }
            else if(datatype_img1 == V3D_UINT16)
            {
                //
                unsigned short* data1d = NULL;
                try
                {
                    data1d = new unsigned short [totalplxs];

                    memset(data1d, 0, sizeof(unsigned short)*totalplxs);
                }
                catch(...)
                {
                    printf("Fail to allocate memory.\n");
                    return -1;
                }

                V3DLONG c1=0, c2=0;

                for(V3DLONG c=0; c<colordim-1; c++)
                {
                    V3DLONG offset_c = c*pagesz;

                    V3DLONG offset_c1, offset_c2;
                    bool b_img1;

                    if(c1<sz_img1[3])
                    {
                        b_img1 = true;

                        if(b_img1existNULL)
                        {
                            if(c1!=nullcolor1)
                            {
                                offset_c1 = c1*pagesz;
                            }
                            else
                            {
                                c1++;

                                if(c1<sz_img1[3])
                                    offset_c1 = c1*pagesz;
                                else
                                    b_img1 = false;
                            }
                        }

                        if(c1!=ref1)
                        {
                            offset_c1 = c1*pagesz;
                        }
                        else
                        {
                            c1++;

                            if(c1<sz_img1[3])
                                offset_c1 = c1*pagesz;
                            else
                                b_img1 = false;
                        }

                        qDebug()<<"color 1 ..."<<c1<<c;

                        c1++;
                    }
                    else
                    {
                        b_img1 = false;
                    }

                    if(!b_img1)
                    {
                        // assume image2 is single float deformed signal channel
                        qDebug()<<"color 2 ..."<<c2<<c;

                        offset_c2=0;
                    }

                    for (V3DLONG k=0; k<sz_img1[2]; k++)
                    {
                        V3DLONG offset_k = offset_c + k*sz_img1[0]*sz_img1[1];

                        V3DLONG offset_k1 = offset_k - offset_c + offset_c1;
                        V3DLONG offset_k2 = offset_k - offset_c + offset_c2;

                        for(V3DLONG j=0; j<sz_img1[1]; j++)
                        {
                            V3DLONG offset_j = offset_k + j*sz_img1[0];

                            V3DLONG offset_j1 = offset_k1 + j*sz_img1[0];
                            V3DLONG offset_j2 = offset_k2 + j*sz_img1[0];

                            for(V3DLONG i=0; i<sz_img1[0]; i++)
                            {
                                V3DLONG idx = offset_j + i;

                                if (b_img1)
                                {
                                    data1d[idx] = ((unsigned short *)p1dImg1)[offset_j1 + i];
                                }
                                else
                                {
                                    data1d[idx] = ((unsigned short*)p)[offset_j2 + i];
                                }
                            }
                        }
                    }
                }

                V3DLONG offset = (colordim-1)*pagesz;
                V3DLONG offset1 = ref1*pagesz;

                for(V3DLONG i=0; i<pagesz; i++)
                {
                    data1d[offset + i] = ((unsigned short *)p1dImg1)[offset1+i];
                }

                // step 5. cutting the blank plane introduced by blending
                unsigned short *pOutput = NULL;
                V3DLONG szOutput[4];

                img_cutting<unsigned short>( (unsigned short*)data1d, sz_blend, pOutput, szOutput);

                // de-alloc
                if(data1d) {delete []data1d; data1d=NULL;}

                // output
                if(b_saveimage)
                {
                    //save
                    if (saveImage(blendImageName.toStdString().c_str(), (const unsigned char *)pOutput, szOutput, 2)!=true)
                    {
                        fprintf(stderr, "Error happens in file writing. Exit. \n");
                        return false;
                    }

                    //de-alloc
                    if(pOutput) {delete []pOutput; pOutput=NULL;}
                }
                else
                {
                    V3DPluginArgItem arg;

                    arg.type = "data"; arg.p = (void *)(pOutput); output << arg;

                    V3DLONG metaImg[5]; // xyzc datatype
                    metaImg[0] = szOutput[0];
                    metaImg[1] = szOutput[1];
                    metaImg[2] = szOutput[2];
                    metaImg[3] = szOutput[3];
                    metaImg[4] = datatype_img1;

                    arg.type = "metaImage"; arg.p = (void *)(metaImg); output << arg;
                }
            }
            else
            {
                printf("Currently this program only support UINT8, UINT16 datatype.\n");
                return false;
            }

            // de-alloc
            y_del<V3DLONG>(sz_img1);
            y_del<V3DLONG>(sz_img2);
            y_del<V3DLONG>(sz_deformed);
            y_del<void>(p);

            //
            return true;
        }

        //step 3: need to run a simple stitching to figure out the displacement
        int transtype;
        if(!stitch_paired_images_with_refchan(p4DImage1, ref1, p4DImage2, ref2, false, transtype))
        {
            fprintf(stderr, "The stitching step fails and thus return.\n");
            return false;
        }

        // step 4: image blending
        // suppose image1 and image2 have a common reference
        // the blended image color dim = image1 color dim + image2 color dim - 1
        V3DLONG colordim = sz_img1[3]+sz_img2[3]-1;

        if(b_img1existNULL) colordim--;
        if(b_img2existNULL) colordim--;

        V3DLONG sz_blend[4];
        sz_blend[0] = sz_img1[0]; sz_blend[1] = sz_img1[1]; sz_blend[2] = sz_img1[2]; sz_blend[3] = colordim;

        V3DLONG totalplxs = colordim * pagesz;

        if(datatype_img1 == V3D_UINT8)
        {
            //
            unsigned char* data1d = NULL;
            try
            {
                data1d = new unsigned char [totalplxs];

                memset(data1d, 0, sizeof(unsigned char)*totalplxs);
            }
            catch(...)
            {
                printf("Fail to allocate memory.\n");
                return -1;
            }

            //
            V3DLONG c1=0, c2=0;

            for(V3DLONG c=0; c<colordim-1; c++)
            {
                V3DLONG offset_c = c*pagesz;

                V3DLONG offset_c1, offset_c2;
                bool b_img1;

                if(c1<sz_img1[3])
                {
                    b_img1 = true;

                    if(b_img1existNULL)
                    {
                        if(c1!=nullcolor1)
                        {
                            offset_c1 = c1*pagesz;
                        }
                        else
                        {
                            c1++;

                            if(c1<sz_img1[3])
                                offset_c1 = c1*pagesz;
                            else
                                b_img1 = false;
                        }
                    }

                    if(c1!=ref1)
                    {
                        offset_c1 = c1*pagesz;
                    }
                    else
                    {
                        c1++;

                        if(c1<sz_img1[3])
                            offset_c1 = c1*pagesz;
                        else
                            b_img1 = false;
                    }

                    qDebug()<<"color 1 ..."<<c1<<c;

                    c1++;
                }
                else
                {
                    b_img1 = false;
                }

                if(!b_img1)
                {
                    if(b_img2existNULL)
                    {
                        if(c2!=nullcolor2)
                        {
                            offset_c2 = c2*pagesz;
                        }
                        else
                        {
                            c2++;

                            if(c2<sz_img2[3])
                                offset_c2 = c2*pagesz;
                            else
                                continue;
                        }
                    }

                    if(c2!=ref2)
                    {
                        offset_c2 = c2*pagesz;
                    }
                    else
                    {
                        c2++;

                        if(c2<sz_img2[3])
                            offset_c2 = c2*pagesz;
                        else
                            continue;
                    }

                    qDebug()<<"color 2 ..."<<c2<<c;

                    c2++;
                }

                for (V3DLONG k=0; k<sz_img1[2]; k++)
                {
                    V3DLONG offset_k = offset_c + k*sz_img1[0]*sz_img1[1];

                    V3DLONG offset_k1 = offset_k - offset_c + offset_c1;
                    V3DLONG offset_k2 = offset_k - offset_c + offset_c2;

                    for(V3DLONG j=0; j<sz_img1[1]; j++)
                    {
                        V3DLONG offset_j = offset_k + j*sz_img1[0];

                        V3DLONG offset_j1 = offset_k1 + j*sz_img1[0];
                        V3DLONG offset_j2 = offset_k2 + j*sz_img1[0];

                        for(V3DLONG i=0; i<sz_img1[0]; i++)
                        {
                            V3DLONG idx = offset_j + i;

                            if (b_img1)
                            {
                                data1d[idx] = p1dImg1[offset_j1 + i];
                            }
                            else
                            {
                                data1d[idx] = p1dImg2[offset_j2 + i];
                            }
                        }
                    }
                }
            }

            V3DLONG offset = (colordim-1)*pagesz;
            V3DLONG offset1 = ref1*pagesz;
            //V3DLONG offset2 = ref2*pagesz;

            for(V3DLONG i=0; i<pagesz; i++)
            {
                //data1d[offset + i] = 0.5*p1dImg1[offset1+i] + 0.5*p1dImg2[offset2+i];
                data1d[offset + i] = p1dImg1[offset1 + i];
            }

            // step 5. cutting the blank plane introduced by blending
            unsigned char *pOutput = NULL;
            V3DLONG szOutput[4];

            img_cutting<unsigned char>( (unsigned char*)data1d, sz_blend, pOutput, szOutput);

            // de-alloc
            if(data1d) {delete []data1d; data1d=NULL;}

            // output
            if(b_saveimage)
            {
                //save
                if (saveImage(blendImageName.toStdString().c_str(), (const unsigned char *)pOutput, szOutput, 1)!=true)
                {
                    fprintf(stderr, "Error happens in file writing. Exit. \n");
                    return false;
                }

                //de-alloc
                if(pOutput) {delete []pOutput; pOutput=NULL;}
            }
            else
            {
                V3DPluginArgItem arg;

                arg.type = "data"; arg.p = (void *)(pOutput); output << arg;

                V3DLONG metaImg[5]; // xyzc datatype
                metaImg[0] = szOutput[0];
                metaImg[1] = szOutput[1];
                metaImg[2] = szOutput[2];
                metaImg[3] = szOutput[3];
                metaImg[4] = datatype_img1;

                arg.type = "metaImage"; arg.p = (void *)(metaImg); output << arg;
            }

        }
        else if(datatype_img1 == V3D_UINT16)
        {
            //
            unsigned short* data1d = NULL;
            try
            {
                data1d = new unsigned short [totalplxs];

                memset(data1d, 0, sizeof(unsigned short)*totalplxs);
            }
            catch(...)
            {
                printf("Fail to allocate memory.\n");
                return -1;
            }

            //
            V3DLONG c1=0, c2=0;

            for(V3DLONG c=0; c<colordim-1; c++)
            {
                V3DLONG offset_c = c*pagesz;

                V3DLONG offset_c1, offset_c2;
                bool b_img1;

                if(c1<sz_img1[3])
                {
                    b_img1 = true;

                    if(b_img1existNULL)
                    {
                        if(c1!=nullcolor1)
                        {
                            offset_c1 = c1*pagesz;
                        }
                        else
                        {
                            c1++;

                            if(c1<sz_img1[3])
                                offset_c1 = c1*pagesz;
                            else
                                b_img1 = false;
                        }
                    }

                    if(c1!=ref1)
                    {
                        offset_c1 = c1*pagesz;
                    }
                    else
                    {
                        c1++;

                        if(c1<sz_img1[3])
                            offset_c1 = c1*pagesz;
                        else
                            b_img1 = false;
                    }

                    qDebug()<<"color 1 ..."<<c1<<c;

                    c1++;
                }
                else
                {
                    b_img1 = false;
                }

                if(!b_img1)
                {
                    if(b_img2existNULL)
                    {
                        if(c2!=nullcolor2)
                        {
                            offset_c2 = c2*pagesz;
                        }
                        else
                        {
                            c2++;

                            if(c2<sz_img2[3])
                                offset_c2 = c2*pagesz;
                            else
                                continue;
                        }
                    }

                    if(c2!=ref2)
                    {
                        offset_c2 = c2*pagesz;
                    }
                    else
                    {
                        c2++;

                        if(c2<sz_img2[3])
                            offset_c2 = c2*pagesz;
                        else
                            continue;
                    }

                    qDebug()<<"color 2 ..."<<c2<<c;

                    c2++;
                }

                for (V3DLONG k=0; k<sz_img1[2]; k++)
                {
                    V3DLONG offset_k = offset_c + k*sz_img1[0]*sz_img1[1];

                    V3DLONG offset_k1 = offset_k - offset_c + offset_c1;
                    V3DLONG offset_k2 = offset_k - offset_c + offset_c2;

                    for(V3DLONG j=0; j<sz_img1[1]; j++)
                    {
                        V3DLONG offset_j = offset_k + j*sz_img1[0];

                        V3DLONG offset_j1 = offset_k1 + j*sz_img1[0];
                        V3DLONG offset_j2 = offset_k2 + j*sz_img1[0];

                        for(V3DLONG i=0; i<sz_img1[0]; i++)
                        {
                            V3DLONG idx = offset_j + i;

                            if (b_img1)
                            {
                                data1d[idx] = ((unsigned short *)p1dImg1)[offset_j1 + i];
                            }
                            else
                            {
                                data1d[idx] = ((unsigned short *)p1dImg2)[offset_j2 + i];
                            }
                        }
                    }
                }
            }

            V3DLONG offset = (colordim-1)*pagesz;
            V3DLONG offset1 = ref1*pagesz;
            //V3DLONG offset2 = ref2*pagesz;

            for(V3DLONG i=0; i<pagesz; i++)
            {
                //data1d[offset + i] = 0.5*((unsigned short *)p1dImg1)[offset1+i] + 0.5*((unsigned short *)p1dImg2)[offset2+i];
                data1d[offset + i] = ((unsigned short *)p1dImg1)[offset1 + i];
            }

            // step 5. cutting the blank plane introduced by blending
            unsigned short *pOutput = NULL;
            V3DLONG szOutput[4];

            img_cutting<unsigned short>( (unsigned short*)data1d, sz_blend, pOutput, szOutput);

            // de-alloc
            if(data1d) {delete []data1d; data1d=NULL;}

            // output
            if(b_saveimage)
            {
                //save
                if (saveImage(blendImageName.toStdString().c_str(), (const unsigned char *)pOutput, szOutput, 2)!=true)
                {
                    fprintf(stderr, "Error happens in file writing. Exit. \n");
                    return false;
                }

                //de-alloc
                if(pOutput) {delete []pOutput; pOutput=NULL;}
            }
            else
            {
                V3DPluginArgItem arg;

                arg.type = "data"; arg.p = (void *)(pOutput); output << arg;

                V3DLONG metaImg[5]; // xyzc datatype
                metaImg[0] = szOutput[0];
                metaImg[1] = szOutput[1];
                metaImg[2] = szOutput[2];
                metaImg[3] = szOutput[3];
                metaImg[4] = datatype_img1;

                arg.type = "metaImage"; arg.p = (void *)(metaImg); output << arg;
            }
        }
        else
        {
            printf("Currently this program only support UINT8, UINT16 datatype.\n");
            return false;
        }

        // de-alloc
        if(sz_img1) {delete []sz_img1; sz_img1=NULL;}
        if(sz_img2) {delete []sz_img2; sz_img2=NULL;}

    }
    else if (func_name == tr("transformdet"))
    {
        if(input.size()<1) return false; // no inputs

        vector<char*> * infilelist = (vector<char*> *)(input.at(0).p);
        vector<char*> * paralist;
        vector<char*> * outfilelist;
        if(infilelist->empty())
        {
            //print Help info
            printf("\nUsage: v3d -x blend_multiscanstacks.dylib -f transformdet -i <input_images> -o <output_transform_type> \
                   -p \"#k <b_morecolorstack_first nonzero(true)/zero(false)>\" \n");

            return true;
        }

        if(infilelist->size()<2)
        {
            printf("\nThe multiscan blending program needs two images as input!\n");

            return false;
        }

        qDebug()<<"input files ..."<<infilelist->at(0)<<infilelist->at(1);

        char * infile = infilelist->at(0); // input images
        char * paras = NULL; // parameters
        char * outfile = NULL; // outputs

        if(output.size()>0) { outfilelist = (vector<char*> *)(output.at(0).p); outfile = outfilelist->at(0);}  // specify output
        if(input.size()>1) { paralist = (vector<char*> *)(input.at(1).p); paras =  paralist->at(0);} // parameters

        bool b_morecolorstack_first = false; // keep inputs order by default

        if(paras)
        {
            int argc = 0;
            int len = strlen(paras);
            int posb[200];
            char * myparas = new char[len];
            strcpy(myparas, paras);
            for(int i = 0; i < len; i++)
            {
                if(i==0 && myparas[i] != ' ' && myparas[i] != '\t')
                {
                    posb[argc++] = i;
                }
                else if((myparas[i-1] == ' ' || myparas[i-1] == '\t') && (myparas[i] != ' ' && myparas[i] != '\t'))
                {
                    posb[argc++] = i;
                }
            }
            char ** argv = new char* [argc];
            for(int i = 0; i < argc; i++)
            {
                argv[i] = myparas + posb[i];
            }
            for(int i = 0; i < len; i++)
            {
                if(myparas[i]==' ' || myparas[i]=='\t')
                    myparas[i]='\0';
            }

            char* key;
            for(int i=0; i<argc; i++)
            {
                if(i+1 != argc) // check that we haven't finished parsing yet
                {
                    key = argv[i];

                    qDebug()<<">>key ..."<<key;

                    if (*key == '#')
                    {
                        while(*++key)
                        {
                            if (!strcmp(key, "k"))
                            {
                                b_morecolorstack_first = (atoi( argv[i+1] )) ? true:false;
                                printf("Now set the b_morecolorstack_first = %s\n", (b_morecolorstack_first)? "TRUE" : "FALSE");
                                i++;
                            }
                            else if (!strcmp(key, "r"))
                            {
                                ref_chans[0] = atoi( argv[++i] );
                                ref_chans[1] = atoi( argv[++i] );
                                b_override_channel = true;
                                printf("Using channels %d and %d as blending channels.\n", ref_chans[0], ref_chans[1]);
                                i++;
                            }
                            else
                            {
                                cout<<"parsing ..."<<key<<" "<<i<<" "<<"Unknown command. Type 'v3d -x plugin_name -f function_name' for usage"<<endl;
                                return false;
                            }

                        }
                    }
                    else
                    {
                        cout<<"parsing ..."<<key<<" "<<i<<" "<<"Unknown command. Type 'v3d -x plugin_name -f function_name' for usage"<<endl;
                        return false;
                    }

                }
            }
        }

        QString transformTypeName;
        QString m_InputFileName(infile);
        m_InputFileName.chop(4);
        if(!outfile)
            transformTypeName = QFileInfo(m_InputFileName).path().append(SEPCHAR).append("TransformationType.txt");
        else
            transformTypeName = QString(outfile);

        // image blending
        QString m_InputFileName1(infilelist->at(0));
        QString m_InputFileName2(infilelist->at(1));

        if ( !QFile::exists(m_InputFileName1) || !QFile::exists(m_InputFileName2))
        {
            cout<<"Image does not exist!"<<endl;
            return false;
        }

        // info reader and color configuration
        bool b_infoloaded = false;
        int *c1=NULL, *c2=NULL;

        // load images
        Image4DSimple p4DImage1, p4DImage2;

        V3DLONG *sz_img1 = 0;
        int datatype_img1 = 0;
        unsigned char* p1dImg1 = 0;

        if(QFileInfo(m_InputFileName1).suffix().toUpper().compare("LSM") == 0)
        {
            p4DImage1.loadImage(const_cast<char *>(m_InputFileName1.toStdString().c_str()), true); // Mylib

            b_morecolorstack_first = true;
        }
        else
        {
            p4DImage1.loadImage(const_cast<char *>(m_InputFileName1.toStdString().c_str()), false); // libtiff
        }

        p1dImg1 = p4DImage1.getRawData();

        sz_img1 = new V3DLONG [4];

        sz_img1[0] = p4DImage1.getXDim();
        sz_img1[1] = p4DImage1.getYDim();
        sz_img1[2] = p4DImage1.getZDim();
        sz_img1[3] = p4DImage1.getCDim();

        datatype_img1 = p4DImage1.getUnitBytes();

        V3DLONG *sz_img2 = 0;
        int datatype_img2 = 0;
        unsigned char* p1dImg2 = 0;

        if(QFileInfo(m_InputFileName2).suffix().toUpper().compare("LSM") == 0)
        {
            p4DImage2.loadImage(const_cast<char *>(m_InputFileName2.toStdString().c_str()), true); // Mylib

            b_morecolorstack_first = true;
        }
        else
        {
            p4DImage2.loadImage(const_cast<char *>(m_InputFileName2.toStdString().c_str()), false); // libtiff
        }

        p1dImg2 = p4DImage2.getRawData();

        sz_img2 = new V3DLONG [4];

        sz_img2[0] = p4DImage2.getXDim();
        sz_img2[1] = p4DImage2.getYDim();
        sz_img2[2] = p4DImage2.getZDim();
        sz_img2[3] = p4DImage2.getCDim();

        datatype_img2 = p4DImage2.getUnitBytes();

        // check dims datatype
        if(datatype_img1 != datatype_img2)
        {
            cout<<"Images are different data types! Do nothing!"<<endl;
            return false;
        }

        if(sz_img1[0] != sz_img2[0] || sz_img1[1] != sz_img2[1] || sz_img1[2] != sz_img2[2] ) // x, y, z
        {
            cout<<"Images are different dimensions! Do nothing!"<<endl;
            return false;
        }

        // swap inputs' order by choosing the input with more color channels as the first input
        if(b_morecolorstack_first && sz_img1[3]<sz_img2[3])
        {
            //
            qDebug()<<"original stack 1 "<<p1dImg1<<sz_img1[0]<<sz_img1[1]<<sz_img1[2]<<sz_img1[3]<<p4DImage1.getTotalUnitNumber();
            qDebug()<<"original stack 2 "<<p1dImg2<<sz_img2[0]<<sz_img2[1]<<sz_img2[2]<<sz_img2[3]<<p4DImage2.getTotalUnitNumber();

            //
            QString tmpName = m_InputFileName1;
            m_InputFileName1 = m_InputFileName2;
            m_InputFileName2 = tmpName;

            //
            unsigned char *p1 = NULL;
            unsigned char *p2 = NULL;

            try
            {
                V3DLONG totalplxs1 = p4DImage1.getTotalBytes();
                p1 = new unsigned char [totalplxs1];
                memcpy(p1, p4DImage1.getRawData(), totalplxs1);

                V3DLONG totalplxs2 = p4DImage2.getTotalBytes();
                p2 = new unsigned char [totalplxs2];
                memcpy(p2, p4DImage2.getRawData(), totalplxs2);
            }
            catch (...)
            {
                cout<<"Fail to allocate memory for swaping temporary pointers."<<endl;
                if (p1) {delete []p1; p1=0;}
                if (p2) {delete []p2; p2=0;}
                return false;
            }

            p4DImage1.setData(p2, sz_img2[0], sz_img2[1], sz_img2[2], sz_img2[3], (ImagePixelType)datatype_img2);
            p4DImage2.setData(p1, sz_img1[0], sz_img1[1], sz_img1[2], sz_img1[3], (ImagePixelType)datatype_img1);

            //
            p1dImg1 = p4DImage1.getRawData();
            sz_img1[3] = p4DImage1.getCDim();

            p1dImg2 = p4DImage2.getRawData();
            sz_img2[3] = p4DImage2.getCDim();

            qDebug()<<"switched stack 1 "<<p1dImg1<< " "<<sz_img1[0]<< " "<<sz_img1[1]<< " "<<sz_img1[2]<< " "<<sz_img1[3]<< " "<<p4DImage1.getTotalUnitNumber();
            qDebug()<<"switched stack 2 "<<p1dImg2<< " "<<sz_img2[0]<< " "<<sz_img2[1]<< " "<<sz_img2[2]<< " "<<sz_img2[3]<< " "<<p4DImage2.getTotalUnitNumber();
        }

//        if( (QFileInfo(m_InputFileName1).suffix().toUpper().compare("LSM") == 0) && (QFileInfo(m_InputFileName2).suffix().toUpper().compare("LSM") == 0) )
//        {
//            Y_LSMINFO<V3DLONG> lsminfo1(m_InputFileName1.toStdString());
//            lsminfo1.loadHeader();

//            c1 = new int [ sz_img1[3] ];
//            for(int i=0; i<sz_img1[3]; i++)
//            {
//                c1[i] = getChannelNum(lsminfo1.colorchannels.at(i).R, lsminfo1.colorchannels.at(i).G, lsminfo1.colorchannels.at(i).B);
//            }

//            Y_LSMINFO<V3DLONG> lsminfo2(m_InputFileName2.toStdString());
//            lsminfo2.loadHeader();

//            c2 = new int [ sz_img2[3] ];
//            for(int i=0; i<sz_img2[3]; i++)
//            {
//                c2[i] = getChannelNum(lsminfo2.colorchannels.at(i).R, lsminfo2.colorchannels.at(i).G, lsminfo2.colorchannels.at(i).B);
//            }

//            b_infoloaded = true;
//        }

        //
        V3DLONG pagesz = sz_img1[0]*sz_img1[1]*sz_img1[2];

        // find reference : suppose reference color channels similar enough
        V3DLONG ref1=0, ref2=0, nullcolor1 = -1, nullcolor2 = -1;
        bool b_img1existNULL=false, b_img2existNULL=false;

        if(b_infoloaded)
        {
            for(int i=0; i<sz_img1[3]; i++)
            {
                if(c1[i]==3) {ref1 = i; break;}
            }

            for(int i=0; i<sz_img2[3]; i++)
            {
                if(c2[i]==3) {ref2 = i; break;}
            }
        }
        else
        {
           if ( b_override_channel )
           {
              ref1 = ref_chans[ 0 ];
              ref2 = ref_chans[ 1 ];
           }
           else
           {
            // step 1: find null color channel
            for(V3DLONG c=0; c<sz_img1[3]; c++) // image 1
            {
                V3DLONG offset_c = c*pagesz;
                V3DLONG sumint1 = 0;
                for (V3DLONG k=0; k<sz_img1[2]; k++)
                {
                    V3DLONG offset_k = offset_c + k*sz_img1[0]*sz_img1[1];
                    for(V3DLONG j=0; j<sz_img1[1]; j++)
                    {
                        V3DLONG offset_j = offset_k + j*sz_img1[0];
                        for(V3DLONG i=0; i<sz_img1[0]; i++)
                        {
                            V3DLONG idx = offset_j + i;

                            if(datatype_img1 == V3D_UINT8)
                            {
                                sumint1 += p1dImg1[idx];
                            }
                            else if(datatype_img1 == V3D_UINT16)
                            {
                                sumint1 += ((unsigned short *)p1dImg1)[idx];
                            }
                            else if(datatype_img1 == V3D_FLOAT32)
                            {
                                sumint1 += ((float *)p1dImg1)[idx];
                            }
                            else
                            {
                                cout<<"Your image datatype is not supported!"<<endl;
                                return false;
                            }
                        }
                    }
                }

                qDebug()<<"sum ..."<<sumint1<<c;

                if(sumint1<EMPTY)
                {
                    b_img1existNULL = true;
                    nullcolor1 = c;
                }
            }

            for(V3DLONG c=0; c<sz_img2[3]; c++) // image 2
            {
                V3DLONG offset_c = c*pagesz;
                V3DLONG sumint2 = 0;
                for (V3DLONG k=0; k<sz_img1[2]; k++)
                {
                    V3DLONG offset_k = offset_c + k*sz_img1[0]*sz_img1[1];
                    for(V3DLONG j=0; j<sz_img1[1]; j++)
                    {
                        V3DLONG offset_j = offset_k + j*sz_img1[0];
                        for(V3DLONG i=0; i<sz_img1[0]; i++)
                        {
                            V3DLONG idx = offset_j + i;

                            if(datatype_img1 == V3D_UINT8)
                            {
                                sumint2 += p1dImg2[idx];
                            }
                            else if(datatype_img1 == V3D_UINT16)
                            {
                                sumint2 += ((unsigned short *)p1dImg2)[idx];
                            }
                            else if(datatype_img1 == V3D_FLOAT32)
                            {
                                sumint2 += ((float *)p1dImg2)[idx];
                            }
                            else
                            {
                                cout<<"Your image datatype is not supported!"<<endl;
                                return false;
                            }
                        }
                    }
                }

                qDebug()<<"sum ..."<<sumint2<<c;

                if(sumint2<EMPTY)
                {
                    b_img2existNULL = true;
                    nullcolor2 = c;
                }
            }

            // step 2: find ref color channel by compute MI
            double scoreMI = -1e10; // -INF
            for(V3DLONG c1=0; c1<sz_img1[3]; c1++)
            {
                if(b_img1existNULL)
                {
                    if ( c1 == nullcolor1 )
                       continue;
                }

                for(V3DLONG c2=0; c2<sz_img2[3]; c2++)
                {
                    if(b_img2existNULL)
                    {
                       if ( c2 == nullcolor2 )
                          continue;
                    }

                    if(datatype_img1 == V3D_UINT8)
                    {
                        unsigned char* pImg1Proxy = p1dImg1 + c1*pagesz;
                        unsigned char* pImg2Proxy = p1dImg2 + c2*pagesz;

                       double valMI = mi_computing< unsigned char >(
                           pImg1Proxy, pImg2Proxy, pagesz, 1 );

                        if(valMI>scoreMI)
                        {
                            scoreMI = valMI;

                            ref1 = c1;
                            ref2 = c2;
                        }
                    }
                    else if(datatype_img1 == V3D_UINT16)
                    {
                       unsigned short *pImg1Proxy =
                           ( (unsigned short *)p1dImg1 ) + c1 * pagesz;
                       unsigned short *pImg2Proxy =
                           ( (unsigned short *)p1dImg2 ) + c2 * pagesz;

                       double valMI = mi_computing< unsigned short >(
                           pImg1Proxy, pImg2Proxy, pagesz, 2 );

                        qDebug()<<"mi ..."<<valMI<<c1<<c2;

                        if(valMI>scoreMI)
                        {
                            scoreMI = valMI;

                            ref1 = c1;
                            ref2 = c2;
                        }
                    }
                    else if(datatype_img1 == V3D_FLOAT32)
                    {
                       printf(
                           "Currently this program dose not support FLOAT32.\n" ); // temporary
                        return false;
                    }
                    else
                    {
                       printf( "Currently this program only support UINT8, UINT16, and "
                               "FLOAT32 datatype.\n" );
                        return false;
                    }
                }
            }
        }
        }
        qDebug() << "ref ..." << ref1 << ref2 << "null color ..." << b_img1existNULL
                 << nullcolor1 << b_img2existNULL << nullcolor2;

        //step 3: detect transformation type
        int transtype;
        if(!stitch_paired_images_with_refchan(p4DImage1, ref1, p4DImage2, ref2, true, transtype))
        {
            fprintf(stderr, "The stitching step fails and thus return.\n");
            return false;
        }

        //step 4: save transformation type
        FILE *pTT=0;

        pTT = fopen(transformTypeName.toStdString().c_str(),"wt");

        fprintf(pTT, "# Transformation Type File: V1.0 \n"); //
        fprintf(pTT, "%d %s transformations.\n\n", transtype, transtype==0?"rigid":"non-rigid");

        fclose(pTT);

    }
    else if (func_name == tr("extractchannels"))
    {
        // extract the reference from 3-color .lsm and save the reference as 'target_ref.v3draw'
        // extract the reference and the signal from 2-color .lsm and save them as 'subject_ref.v3draw' and 'subject_signal.v3draw'

        if(input.size()<1) return false; // no inputs

        vector<char*> * infilelist = (vector<char*> *)(input.at(0).p);
        vector<char*> * paralist;
        vector<char*> * outfilelist;
        if(infilelist->empty())
        {
            //print Help info
            printf("\nUsage: v3d -x blend_multiscanstacks.dylib -f extractchannels -i <input_images> \
                   -p \"#k <b_morecolorstack_first nonzero(true)/zero(false)>\" \n");

            return true;
        }

        if(infilelist->size()<2)
        {
            printf("\nThe multiscan blending program needs two images as input!\n");

            return false;
        }

        qDebug()<<"input files ..."<<infilelist->at(0)<<infilelist->at(1);

        char * infile = infilelist->at(0); // input images
        char * paras = NULL; // parameters
        char * outfile = NULL; // outputs

        if(output.size()>0) { outfilelist = (vector<char*> *)(output.at(0).p); outfile = outfilelist->at(0);}  // specify output
        if(input.size()>1) { paralist = (vector<char*> *)(input.at(1).p); paras =  paralist->at(0);} // parameters

        bool b_saveimage = true; // save the blended image by default
        bool b_morecolorstack_first = false; // keep inputs order by default

        if(paras)
        {
            int argc = 0;
            int len = strlen(paras);
            int posb[200];
            char * myparas = new char[len];
            strcpy(myparas, paras);
            for(int i = 0; i < len; i++)
            {
                if(i==0 && myparas[i] != ' ' && myparas[i] != '\t')
                {
                    posb[argc++] = i;
                }
                else if((myparas[i-1] == ' ' || myparas[i-1] == '\t') && (myparas[i] != ' ' && myparas[i] != '\t'))
                {
                    posb[argc++] = i;
                }
            }
            char ** argv = new char* [argc];
            for(int i = 0; i < argc; i++)
            {
                argv[i] = myparas + posb[i];
            }
            for(int i = 0; i < len; i++)
            {
                if(myparas[i]==' ' || myparas[i]=='\t')
                    myparas[i]='\0';
            }

            char* key;
            for(int i=0; i<argc; i++)
            {
                if(i+1 != argc) // check that we haven't finished parsing yet
                {
                    key = argv[i];

                    qDebug()<<">>key ..."<<key;

                    if (*key == '#')
                    {
                        while(*++key)
                        {
                            if (!strcmp(key, "k"))
                            {
                                b_morecolorstack_first = (atoi( argv[i+1] )) ? true:false;
                                printf("Now set the b_morecolorstack_first = %s\n", (b_morecolorstack_first)? "TRUE" : "FALSE");
                                i++;
                            }
                            else if (!strcmp(key, "r"))
                            {
                                ref_chans[0] = atoi( argv[++i] );
                                ref_chans[1] = atoi( argv[++i] );
                                b_override_channel = true;
                                printf("Using channels %d and %d as blending channels.\n", ref_chans[0], ref_chans[1]);
                                i++;
                            }
                            else
                            {
                                cout<<"parsing ..."<<key<<" "<<i<<" "<<"Unknown command. Type 'v3d -x plugin_name -f function_name' for usage"<<endl;
                                return false;
                            }

                        }
                    }
                    else
                    {
                        cout<<"parsing ..."<<key<<" "<<i<<" "<<"Unknown command. Type 'v3d -x plugin_name -f function_name' for usage"<<endl;
                        return false;
                    }

                }
            }
        }

        // image blending
        QString m_InputFileName1(infilelist->at(0));
        QString m_InputFileName2(infilelist->at(1));

        if ( !QFile::exists(m_InputFileName1) || !QFile::exists(m_InputFileName2))
        {
            cout<<"Image does not exist!"<<endl;
            return false;
        }

        // info reader and color configuration
        bool b_infoloaded = false;
        int *c1=NULL, *c2=NULL;

        // load images
        Image4DSimple p4DImage1, p4DImage2;

        V3DLONG *sz_img1 = 0;
        int datatype_img1 = 0;
        unsigned char* p1dImg1 = 0;

        if(QFileInfo(m_InputFileName1).suffix().toUpper().compare("LSM") == 0)
        {
            p4DImage1.loadImage(const_cast<char *>(m_InputFileName1.toStdString().c_str()), true); // Mylib

            b_morecolorstack_first = true;
        }
        else
        {
            p4DImage1.loadImage(const_cast<char *>(m_InputFileName1.toStdString().c_str()), false); // libtiff
        }

        p1dImg1 = p4DImage1.getRawData();

        sz_img1 = new V3DLONG [4];

        sz_img1[0] = p4DImage1.getXDim();
        sz_img1[1] = p4DImage1.getYDim();
        sz_img1[2] = p4DImage1.getZDim();
        sz_img1[3] = p4DImage1.getCDim();

        datatype_img1 = p4DImage1.getUnitBytes();

        V3DLONG *sz_img2 = 0;
        int datatype_img2 = 0;
        unsigned char* p1dImg2 = 0;

        if(QFileInfo(m_InputFileName2).suffix().toUpper().compare("LSM") == 0)
        {
            p4DImage2.loadImage(const_cast<char *>(m_InputFileName2.toStdString().c_str()), true); // Mylib

            b_morecolorstack_first = true;
        }
        else
        {
            p4DImage2.loadImage(const_cast<char *>(m_InputFileName2.toStdString().c_str()), false); // libtiff
        }

        p1dImg2 = p4DImage2.getRawData();

        sz_img2 = new V3DLONG [4];

        sz_img2[0] = p4DImage2.getXDim();
        sz_img2[1] = p4DImage2.getYDim();
        sz_img2[2] = p4DImage2.getZDim();
        sz_img2[3] = p4DImage2.getCDim();

        datatype_img2 = p4DImage2.getUnitBytes();

        // check dims datatype
        if(datatype_img1 != datatype_img2)
        {
            cout<<"Images are different data types! Do nothing!"<<endl;
            return false;
        }

        if(sz_img1[0] != sz_img2[0] || sz_img1[1] != sz_img2[1] || sz_img1[2] != sz_img2[2] ) // x, y, z
        {
            cout<<"Images are different dimensions! Do nothing!"<<endl;
            return false;
        }

        // swap inputs' order by choosing the input with more color channels as the first input
        if(b_morecolorstack_first && sz_img1[3]<sz_img2[3])
        {
            //
            qDebug()<<"original stack 1 "<<p1dImg1<<sz_img1[0]<<sz_img1[1]<<sz_img1[2]<<sz_img1[3]<<p4DImage1.getTotalUnitNumber();
            qDebug()<<"original stack 2 "<<p1dImg2<<sz_img2[0]<<sz_img2[1]<<sz_img2[2]<<sz_img2[3]<<p4DImage2.getTotalUnitNumber();

            //
            QString tmpName = m_InputFileName1;
            m_InputFileName1 = m_InputFileName2;
            m_InputFileName2 = tmpName;

            //
            unsigned char *p1 = NULL;
            unsigned char *p2 = NULL;

            try
            {
                V3DLONG totalplxs1 = p4DImage1.getTotalBytes();
                p1 = new unsigned char [totalplxs1];
                memcpy(p1, p4DImage1.getRawData(), totalplxs1);

                V3DLONG totalplxs2 = p4DImage2.getTotalBytes();
                p2 = new unsigned char [totalplxs2];
                memcpy(p2, p4DImage2.getRawData(), totalplxs2);
            }
            catch (...)
            {
                cout<<"Fail to allocate memory for swaping temporary pointers."<<endl;
                if (p1) {delete []p1; p1=0;}
                if (p2) {delete []p2; p2=0;}
                return false;
            }

            p4DImage1.setData(p2, sz_img2[0], sz_img2[1], sz_img2[2], sz_img2[3], (ImagePixelType)datatype_img2);
            p4DImage2.setData(p1, sz_img1[0], sz_img1[1], sz_img1[2], sz_img1[3], (ImagePixelType)datatype_img1);

            //
            p1dImg1 = p4DImage1.getRawData();
            sz_img1[3] = p4DImage1.getCDim();

            p1dImg2 = p4DImage2.getRawData();
            sz_img2[3] = p4DImage2.getCDim();

            qDebug()<<"switched stack 1 "<<p1dImg1<< " "<<sz_img1[0]<< " "<<sz_img1[1]<< " "<<sz_img1[2]<< " "<<sz_img1[3]<< " "<<p4DImage1.getTotalUnitNumber();
            qDebug()<<"switched stack 2 "<<p1dImg2<< " "<<sz_img2[0]<< " "<<sz_img2[1]<< " "<<sz_img2[2]<< " "<<sz_img2[3]<< " "<<p4DImage2.getTotalUnitNumber();
        }

        if( (QFileInfo(m_InputFileName1).suffix().toUpper().compare("LSM") == 0) && (QFileInfo(m_InputFileName2).suffix().toUpper().compare("LSM") == 0) )
        {
            Y_LSMINFO<V3DLONG> lsminfo1(m_InputFileName1.toStdString());
            lsminfo1.loadHeader();

            c1 = new int [ sz_img1[3] ];
            for(int i=0; i<sz_img1[3]; i++)
            {
                c1[i] = getChannelNum(lsminfo1.colorchannels.at(i).R, lsminfo1.colorchannels.at(i).G, lsminfo1.colorchannels.at(i).B);
            }

            Y_LSMINFO<V3DLONG> lsminfo2(m_InputFileName2.toStdString());
            lsminfo2.loadHeader();

            c2 = new int [ sz_img2[3] ];
            for(int i=0; i<sz_img2[3]; i++)
            {
                c2[i] = getChannelNum(lsminfo2.colorchannels.at(i).R, lsminfo2.colorchannels.at(i).G, lsminfo2.colorchannels.at(i).B);
            }

            b_infoloaded = true;
        }

        //
        V3DLONG pagesz = sz_img1[0]*sz_img1[1]*sz_img1[2];

        // find reference : suppose reference color channels similar enough
        V3DLONG ref1=0, ref2=0, nullcolor1 = -1, nullcolor2 = -1;
        bool b_img1existNULL=false, b_img2existNULL=false;

        if(b_infoloaded)
        {
            for(int i=0; i<sz_img1[3]; i++)
            {
                if(c1[i]==3) {ref1 = i; break;}
            }

            for(int i=0; i<sz_img2[3]; i++)
            {
                if(c2[i]==3) {ref2 = i; break;}
            }
        }
        else
        {
           if ( b_override_channel )
           {
              ref1 = ref_chans[ 0 ];
              ref2 = ref_chans[ 1 ];
           }
           else
           {
            // step 1: find null color channel
            for(V3DLONG c=0; c<sz_img1[3]; c++) // image 1
            {
                V3DLONG offset_c = c*pagesz;
                V3DLONG sumint1 = 0;
                for (V3DLONG k=0; k<sz_img1[2]; k++)
                {
                    V3DLONG offset_k = offset_c + k*sz_img1[0]*sz_img1[1];
                    for(V3DLONG j=0; j<sz_img1[1]; j++)
                    {
                        V3DLONG offset_j = offset_k + j*sz_img1[0];
                        for(V3DLONG i=0; i<sz_img1[0]; i++)
                        {
                            V3DLONG idx = offset_j + i;

                            if(datatype_img1 == V3D_UINT8)
                            {
                                sumint1 += p1dImg1[idx];
                            }
                            else if(datatype_img1 == V3D_UINT16)
                            {
                                sumint1 += ((unsigned short *)p1dImg1)[idx];
                            }
                            else if(datatype_img1 == V3D_FLOAT32)
                            {
                                sumint1 += ((float *)p1dImg1)[idx];
                            }
                            else
                            {
                                cout<<"Your image datatype is not supported!"<<endl;
                                return false;
                            }
                        }
                    }
                }

                qDebug()<<"sum ..."<<sumint1<<c;

                if(sumint1<EMPTY)
                {
                    b_img1existNULL = true;
                    nullcolor1 = c;
                }
            }

            for(V3DLONG c=0; c<sz_img2[3]; c++) // image 2
            {
                V3DLONG offset_c = c*pagesz;
                V3DLONG sumint2 = 0;
                for (V3DLONG k=0; k<sz_img1[2]; k++)
                {
                    V3DLONG offset_k = offset_c + k*sz_img1[0]*sz_img1[1];
                    for(V3DLONG j=0; j<sz_img1[1]; j++)
                    {
                        V3DLONG offset_j = offset_k + j*sz_img1[0];
                        for(V3DLONG i=0; i<sz_img1[0]; i++)
                        {
                            V3DLONG idx = offset_j + i;

                            if(datatype_img1 == V3D_UINT8)
                            {
                                sumint2 += p1dImg2[idx];
                            }
                            else if(datatype_img1 == V3D_UINT16)
                            {
                                sumint2 += ((unsigned short *)p1dImg2)[idx];
                            }
                            else if(datatype_img1 == V3D_FLOAT32)
                            {
                                sumint2 += ((float *)p1dImg2)[idx];
                            }
                            else
                            {
                                cout<<"Your image datatype is not supported!"<<endl;
                                return false;
                            }
                        }
                    }
                }

                qDebug()<<"sum ..."<<sumint2<<c;

                if(sumint2<EMPTY)
                {
                    b_img2existNULL = true;
                    nullcolor2 = c;
                }
            }

            // step 2: find ref color channel by compute MI
            double scoreMI = -1e10; // -INF
            for(V3DLONG c1=0; c1<sz_img1[3]; c1++)
            {
                if(b_img1existNULL)
                {
                    if ( c1 == nullcolor1 )
                       continue;
                }

                for(V3DLONG c2=0; c2<sz_img2[3]; c2++)
                {
                    if(b_img2existNULL)
                    {
                       if ( c2 == nullcolor2 )
                          continue;
                    }

                    if(datatype_img1 == V3D_UINT8)
                    {
                        unsigned char* pImg1Proxy = p1dImg1 + c1*pagesz;
                        unsigned char* pImg2Proxy = p1dImg2 + c2*pagesz;

                       double valMI = mi_computing< unsigned char >(
                           pImg1Proxy, pImg2Proxy, pagesz, 1 );

                        if(valMI>scoreMI)
                        {
                            scoreMI = valMI;

                            ref1 = c1;
                            ref2 = c2;
                        }
                    }
                    else if(datatype_img1 == V3D_UINT16)
                    {
                       unsigned short *pImg1Proxy =
                           ( (unsigned short *)p1dImg1 ) + c1 * pagesz;
                       unsigned short *pImg2Proxy =
                           ( (unsigned short *)p1dImg2 ) + c2 * pagesz;

                       double valMI = mi_computing< unsigned short >(
                           pImg1Proxy, pImg2Proxy, pagesz, 2 );

                        qDebug()<<"mi ..."<<valMI<<c1<<c2;

                        if(valMI>scoreMI)
                        {
                            scoreMI = valMI;

                            ref1 = c1;
                            ref2 = c2;
                        }
                    }
                    else if(datatype_img1 == V3D_FLOAT32)
                    {
                       printf(
                           "Currently this program dose not support FLOAT32.\n" ); // temporary
                        return false;
                    }
                    else
                    {
                       printf( "Currently this program only support UINT8, UINT16, and "
                               "FLOAT32 datatype.\n" );
                        return false;
                    }
                }
            }
        }
        }
        qDebug() << "ref ..." << ref1 << ref2 << "null color ..." << b_img1existNULL
                 << nullcolor1 << b_img2existNULL << nullcolor2;

        //step 3: save
        V3DLONG sz_output[4];
        sz_output[0] = sz_img1[0]; sz_output[1] = sz_img1[1]; sz_output[2] = sz_img1[2]; sz_output[3] = 1;

        QString savingName;

        //
        QString qs_filename_output=QString(outfile);

        //
        if(datatype_img1 == V3D_UINT8)
        {
            //
            unsigned char* data1d = NULL;
            try
            {
                y_new<unsigned char, V3DLONG>(data1d, pagesz);

                memset(data1d, 0, sizeof(unsigned char)*pagesz);
            }
            catch(...)
            {
                printf("Fail to allocate memory.\n");
                return -1;
            }

            if(outfile)
            {
                savingName=QFileInfo(qs_filename_output).path().append(SEPCHAR).append("target_ref.v3draw");
            }
            else
            {
                savingName=QFileInfo(m_InputFileName1).path().append(SEPCHAR).append("target_ref.v3draw");;
            }

            //
            V3DLONG offset1 = ref1*pagesz;
            for(V3DLONG i=0; i<pagesz; i++)
            {
                data1d[i] = p1dImg1[offset1+i];
            }

            //
            if(b_saveimage)
            {
                //save
                if (saveImage(savingName.toStdString().c_str(), (const unsigned char *)data1d, sz_output, 1)!=true)
                {
                    fprintf(stderr, "Error happens in file writing. Exit. \n");
                    return false;
                }
            }

            //
            if(outfile)
            {
                savingName=QFileInfo(qs_filename_output).path().append(SEPCHAR).append("subject_ref.v3draw");
            }
            else
            {
                savingName=QFileInfo(m_InputFileName1).path().append(SEPCHAR).append("subject_ref.v3draw");;
            }

            //
            V3DLONG offset2 = ref2*pagesz;
            for(V3DLONG i=0; i<pagesz; i++)
            {
                data1d[i] = p1dImg2[offset2+i];
            }

            //
            if(b_saveimage)
            {
                //save
                if (saveImage(savingName.toStdString().c_str(), (const unsigned char *)data1d, sz_output, 1)!=true)
                {
                    fprintf(stderr, "Error happens in file writing. Exit. \n");
                    return false;
                }
            }

            //
            if(outfile)
            {
                savingName=QFileInfo(qs_filename_output).path().append(SEPCHAR).append("subject_signal.v3draw");
            }
            else
            {
                savingName=QFileInfo(m_InputFileName1).path().append(SEPCHAR).append("subject_signal.v3draw");;
            }

            //
            offset2 = (1-ref2)*pagesz;
            for(V3DLONG i=0; i<pagesz; i++)
            {
                data1d[i] = p1dImg2[offset2+i];
            }

            //
            if(b_saveimage)
            {
                //save
                if (saveImage(savingName.toStdString().c_str(), (const unsigned char *)data1d, sz_output, 1)!=true)
                {
                    fprintf(stderr, "Error happens in file writing. Exit. \n");
                    return false;
                }
            }

            //de-alloc
            y_del<unsigned char>(data1d);

        }
        else if(datatype_img1 == V3D_UINT16)
        {
            //
            unsigned short* data1d = NULL;
            try
            {
                y_new<unsigned short, V3DLONG>(data1d, pagesz);

                memset(data1d, 0, sizeof(unsigned short)*pagesz);
            }
            catch(...)
            {
                printf("Fail to allocate memory.\n");
                return -1;
            }

            if(outfile)
            {
                savingName=QFileInfo(qs_filename_output).path().append(SEPCHAR).append("target_ref.v3draw");
            }
            else
            {
                savingName=QFileInfo(m_InputFileName1).path().append(SEPCHAR).append("target_ref.v3draw");;
            }

            //
            V3DLONG offset1 = ref1*pagesz;
            for(V3DLONG i=0; i<pagesz; i++)
            {
                data1d[i] = ((unsigned short *)p1dImg1)[offset1+i];
            }

            //
            if(b_saveimage)
            {
                //save
                if (saveImage(savingName.toStdString().c_str(), (const unsigned char *)data1d, sz_output, 2)!=true)
                {
                    fprintf(stderr, "Error happens in file writing. Exit. \n");
                    return false;
                }
            }

            //
            if(outfile)
            {
                savingName=QFileInfo(qs_filename_output).path().append(SEPCHAR).append("subject_ref.v3draw");
            }
            else
            {
                savingName=QFileInfo(m_InputFileName1).path().append(SEPCHAR).append("subject_ref.v3draw");;
            }

            //
            V3DLONG offset2 = ref2*pagesz;
            for(V3DLONG i=0; i<pagesz; i++)
            {
                data1d[i] = ((unsigned short *)p1dImg2)[offset2+i];
            }

            //
            if(b_saveimage)
            {
                //save
                if (saveImage(savingName.toStdString().c_str(), (const unsigned char *)data1d, sz_output, 2)!=true)
                {
                    fprintf(stderr, "Error happens in file writing. Exit. \n");
                    return false;
                }
            }

            //
            if(outfile)
            {
                savingName=QFileInfo(qs_filename_output).path().append(SEPCHAR).append("subject_signal.v3draw");
            }
            else
            {
                savingName=QFileInfo(m_InputFileName1).path().append(SEPCHAR).append("subject_signal.v3draw");;
            }

            //
            offset2 = (1-ref2)*pagesz;
            for(V3DLONG i=0; i<pagesz; i++)
            {
                data1d[i] = ((unsigned short *)p1dImg2)[offset2+i];
            }

            //
            if(b_saveimage)
            {
                //save
                if (saveImage(savingName.toStdString().c_str(), (const unsigned char *)data1d, sz_output, 2)!=true)
                {
                    fprintf(stderr, "Error happens in file writing. Exit. \n");
                    return false;
                }
            }

            //de-alloc
            y_del<unsigned short>(data1d);
        }
        else if(datatype_img1 == V3D_FLOAT32) //this section need further improvement, maybe discretization. by PHC, 110810
        {
            //
            float* data1d = NULL;
            try
            {
                y_new<float, V3DLONG>(data1d, pagesz);

                memset(data1d, 0, sizeof(float)*pagesz);
            }
            catch(...)
            {
                printf("Fail to allocate memory.\n");
                return false;
            }

            if(outfile)
            {
                savingName=QFileInfo(qs_filename_output).path().append(SEPCHAR).append("target_ref.v3draw");
            }
            else
            {
                savingName=QFileInfo(m_InputFileName1).path().append(SEPCHAR).append("target_ref.v3draw");;
            }

            //
            V3DLONG offset1 = ref1*pagesz;
            for(V3DLONG i=0; i<pagesz; i++)
            {
                data1d[i] = ((float *)p1dImg1)[offset1+i];
            }

            //
            if(b_saveimage)
            {
                //save
                if (saveImage(savingName.toStdString().c_str(), (const unsigned char *)data1d, sz_output, 4)!=true)
                {
                    fprintf(stderr, "Error happens in file writing. Exit. \n");
                    return false;
                }
            }

            //
            if(outfile)
            {
                savingName=QFileInfo(qs_filename_output).path().append(SEPCHAR).append("subject_ref.v3draw");
            }
            else
            {
                savingName=QFileInfo(m_InputFileName1).path().append(SEPCHAR).append("subject_ref.v3draw");;
            }

            //
            V3DLONG offset2 = ref2*pagesz;
            for(V3DLONG i=0; i<pagesz; i++)
            {
                data1d[i] = ((float *)p1dImg2)[offset2+i];
            }

            //
            if(b_saveimage)
            {
                //save
                if (saveImage(savingName.toStdString().c_str(), (const unsigned char *)data1d, sz_output, 4)!=true)
                {
                    fprintf(stderr, "Error happens in file writing. Exit. \n");
                    return false;
                }
            }

            //
            if(outfile)
            {
                savingName=QFileInfo(qs_filename_output).path().append(SEPCHAR).append("subject_signal.v3draw");
            }
            else
            {
                savingName=QFileInfo(m_InputFileName1).path().append(SEPCHAR).append("subject_signal.v3draw");;
            }

            //
            offset2 = (1-ref2)*pagesz;
            for(V3DLONG i=0; i<pagesz; i++)
            {
                data1d[i] = ((float *)p1dImg2)[offset2+i];
            }

            //
            if(b_saveimage)
            {
                //save
                if (saveImage(savingName.toStdString().c_str(), (const unsigned char *)data1d, sz_output, 4)!=true)
                {
                    fprintf(stderr, "Error happens in file writing. Exit. \n");
                    return false;
                }
            }

            //de-alloc
            y_del<float>(data1d);
        }
        else
        {
            printf("Currently this program only support UINT8, UINT16, FLOAT32 datatype.\n");
            return false;
        }
    }
    else
    {
        printf("\nWrong function specified.\n");
        return false;
    }

    return true;
}

// menu
QStringList ImageBlendPlugin::menulist() const
{
    return QStringList() << tr("Show LSM Info")
                         << tr("Multiscan Image Blending")
                         << tr("About");
}

void ImageBlendPlugin::domenu(const QString &menu_name, V3DPluginCallback2 &callback, QWidget *parent)
{
    if (menu_name == tr("Show LSM Info"))
    {
        // anchor the image to handle
        QString m_FileName = QFileDialog::getOpenFileName(0, QObject::tr("Find a file"), QDir::currentPath(), QObject::tr("Image (*.lsm)"));

        if ( !QFile::exists(m_FileName) )
        {
            cout<<"Image does not exist!"<<endl;
            return;
        }

        string filename = m_FileName.toStdString();

        // image header info
        Y_LSMINFO<V3DLONG> lsminfo(filename);
        lsminfo.loadHeader();

        QTextEdit *lsminfoTxt = new QTextEdit(QString("image: %1 <br>\
                                                      <br><br> Dimensions: \
                                                      <br> x (%2) \
                                                      <br> y (%3) \
                                                      <br> z (%4) \
                                                      <br> c (%5) \
                                                      <br><br> Resolutions: \
                                                      <br> x (%6) um\
                                                      <br> y (%7) um\
                                                      <br> z (%8) um").arg(filename.c_str()).arg(lsminfo.zi.DimensionX).arg(lsminfo.zi.DimensionY).arg(lsminfo.zi.DimensionZ)
                                                      .arg(lsminfo.zi.DimensionChannels).arg(lsminfo.zi.VoxelSizeX*1e6).arg(lsminfo.zi.VoxelSizeY*1e6).arg(lsminfo.zi.VoxelSizeZ*1e6));

                lsminfoTxt->setDocumentTitle("Image Header Info");
        lsminfoTxt->resize(500, 300);
        lsminfoTxt->setReadOnly(true);
        lsminfoTxt->setFontPointSize(12);
        lsminfoTxt->show();

        for(int i=0; i<lsminfo.colorchannels.size(); i++)
        {
            cout<<"colors ..."<<lsminfo.colorchannels.at(i).R<<lsminfo.colorchannels.at(i).G<<lsminfo.colorchannels.at(i).B<<endl;
        }


        //
        return;
    }
    else if (menu_name == tr("Multiscan Image Blending"))
    {
        ImageBlendingDialog dialog(callback, parent, NULL);
        if (dialog.exec()!=QDialog::Accepted)
            return;

        QString m_InputFileName1 = dialog.fn_img1;
        QString m_InputFileName2 = dialog.fn_img2;

        if ( !QFile::exists(m_InputFileName1) )
        {
            cout<<"Image 1 does not exist!"<<endl;
            return;
        }
        if ( !QFile::exists(m_InputFileName2) )
        {
            cout<<"Image 2 does not exist!"<<endl;
            return;
        }

        // call dofunc
        V3DPluginArgItem arg;
        V3DPluginArgList pluginfunc_input;
        V3DPluginArgList pluginfunc_output;

        vector<char*> fileList;
        vector<char*> paraList;

        fileList.clear();
        paraList.clear();

        QByteArray bytes1 = m_InputFileName1.toAscii();
        QByteArray bytes2 = m_InputFileName2.toAscii();

        fileList.push_back(bytes1.data());
        fileList.push_back(bytes2.data());

        paraList.push_back("#s 0");

        arg.type = ""; arg.p = (void *)(&fileList); pluginfunc_input << arg;
        arg.type = ""; arg.p = (void *)(&paraList); pluginfunc_input << arg;

        bool success = dofunc("multiscanblend", pluginfunc_input, pluginfunc_output, callback, parent);

        if(!success)
        {
            QMessageBox::information(parent, "Warning: Image Blending", QString("Fail to run image blending program."));
            return;
        }

        V3DLONG *metaImg = (V3DLONG *)(pluginfunc_output.at(1).p);

        V3DLONG sx = metaImg[0];
        V3DLONG sy = metaImg[1];
        V3DLONG sz = metaImg[2];

        V3DLONG colordim = metaImg[3];
        V3DLONG datatype = metaImg[4];

        // show result in v3d
        if(datatype == V3D_UINT8)
        {
            //
            unsigned char* data1d = (unsigned char *)pluginfunc_output.at(0).p;

            //display
            Image4DSimple p4DImage;
            p4DImage.setData((unsigned char*)data1d, sx, sy, sz, colordim, V3D_UINT8); //

            v3dhandle newwin = callback.newImageWindow();
            callback.setImage(newwin, &p4DImage);
            callback.setImageName(newwin, "Blended Image");
            callback.updateImageWindow(newwin);

        }
        else if(datatype == V3D_UINT16)
        {
            //
            unsigned short* data1d = (unsigned short *)pluginfunc_output.at(0).p;

            //display
            Image4DSimple p4DImage;
            p4DImage.setData((unsigned char*)data1d, sx, sy, sz, colordim, V3D_UINT16); //

            v3dhandle newwin = callback.newImageWindow();
            callback.setImage(newwin, &p4DImage);
            callback.setImageName(newwin, "Blended Image");
            callback.updateImageWindow(newwin);
        }
        else if(datatype == V3D_FLOAT32)
        {
            //
            float* data1d = (float *)pluginfunc_output.at(0).p;;

            //display
            Image4DSimple p4DImage;
            p4DImage.setData((unsigned char*)data1d, sx, sy, sz, colordim, V3D_FLOAT32); //

            v3dhandle newwin = callback.newImageWindow();
            callback.setImage(newwin, &p4DImage);
            callback.setImageName(newwin, "Blended Image");
            callback.updateImageWindow(newwin);
        }
        else
        {
            printf("Currently this program only support UINT8, UINT16, and FLOAT32 datatype.\n");
            return;
        }

    }
    else if (menu_name == tr("About"))
    {
        QMessageBox::information(parent, "Version info", QString("Blend multiple image stacks with various number of color channels, which share a common color channel (reference). Version %1 (July 30, 2011) developed by Yang Yu and Hanchuan Peng. (Janelia Research Farm Campus, HHMI)").arg(getPluginVersion()));
        return;
    }
}

