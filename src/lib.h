/* Las File Decoder - Version 1.2  */
#ifndef LAS_FILE
#define LAS_FILE

#include <fstream>
#include <string>
#include <vector>

class LasFile {
private:
    struct LasHeader {
        uint8_t FileSignatureLASF[4];
        uint16_t fileSourceID,globalEncoding;
        uint32_t guidData1;
        uint16_t guidData2,guidData3;
        uint8_t guidData4[8],versionMaj,versionMin;
        uint8_t systemIdentifier[32],generatingSoftware[32];
        uint16_t genDay,genYear,headerSize;
        uint32_t offsetToPointData,numOfVarLenRecords;
        uint8_t pointDataFormat;
        uint16_t pointDataRecordLen;
        uint32_t numOfPointRecords,numOfPointByReturn[5];
        double x_sca,y_sca,z_sca,x_off,y_off,z_off;
        double x_max,x_min,y_max,y_min,z_max,z_min;
    } __attribute__((packed));
    struct LasPointRecord1 {
        int32_t xpos,ypos,zpos;
        uint16_t intensity;
        uint8_t flags,classification,scanAngleRank,userData;
        uint16_t pointSourceID;
        double gps_time;
    } __attribute__((packed));
    struct LasPointRecord2 {
        int32_t xpos,ypos,zpos;
        uint16_t intensity;
        uint8_t flags,classification,scanAngleRank,userData;
        uint16_t pointSourceID,red,green,blue;
    } __attribute__((packed));
    struct LasPointRecord3 {
        int32_t xpos,ypos,zpos;
        uint16_t intensity;
        uint8_t flags,classification,scanAngleRank,userData;
        uint16_t pointSourceID;
        double gps_time;
        uint16_t red,green,blue;
    } __attribute__((packed));
public:
    LasHeader header;
    LasPointRecord1 *points1;
    LasPointRecord2 *points2;
    LasPointRecord3 *points3;
    std::vector<float> data_stream;
    float deltax,deltay,deltaz;
    LasFile(std::string path) {
        std::ifstream inf(path,std::ios::binary);
        inf.read((char*)&header,sizeof(LasHeader));
        switch ((short)header.pointDataFormat) {
            case 1: {
                points1 = (LasPointRecord1*)malloc(header.numOfPointRecords*sizeof(LasPointRecord1));
                inf.seekg(header.offsetToPointData);
                for (int i=0;i<header.numOfPointRecords;i++) inf.read((char*)&points1[i],sizeof(LasPointRecord1));
                break;
            }
            case 2: {
                points2 = (LasPointRecord2*)malloc(header.numOfPointRecords*sizeof(LasPointRecord2));
                inf.seekg(header.offsetToPointData);
                for (int i=0;i<header.numOfPointRecords;i++) inf.read((char*)&points2[i],sizeof(LasPointRecord2));
                break;
            }
            case 3: {
                points3 = (LasPointRecord3*)malloc(header.numOfPointRecords*sizeof(LasPointRecord3));
                inf.seekg(header.offsetToPointData);
                for (int i=0;i<header.numOfPointRecords;i++) inf.read((char*)&points3[i],sizeof(LasPointRecord3));
                break;
            }
            default: break;
        }
        inf.close();
        deltax = header.x_max-header.x_min;
        deltay = header.y_max-header.y_min;
        deltaz = header.z_max-header.z_min;
        generate_stream();
    }
    void generate_stream() {
        for (int i=0;i<10000;i++) {
            switch((short)header.pointDataFormat) {
                case 1: {
                    data_stream.push_back(points1[i].xpos*header.x_sca+header.x_off);
                    data_stream.push_back(points1[i].ypos*header.y_sca+header.y_off);
                    data_stream.push_back(points1[i].zpos*header.z_sca+header.z_off);
                    // data_stream.push_back(points1[i].xpos*header.x_sca);
                    // data_stream.push_back(points1[i].ypos*header.y_sca);
                    // data_stream.push_back(points1[i].zpos*header.z_sca);
                    data_stream.push_back(points1[i].intensity);
                    data_stream.push_back(points1[i].intensity);
                    data_stream.push_back(points1[i].intensity);
                    break;
                }
                default: break;
            }
        }
    }
    friend std::ostream& operator <<(std::ostream &os, LasFile& lasfile) { 
        os << "v:" << (short)lasfile.header.versionMaj << "." << (short)lasfile.header.versionMin;
        os << " xmin:" << lasfile.header.x_min << " xmax:" << lasfile.header.x_max;
        os << " x1:" << lasfile.data_stream[0];
        os << " col1: [" << lasfile.data_stream[3] << "," << lasfile.data_stream[4] << "," << lasfile.data_stream[5] << "]";
        return os;
    }
};

#endif