//
// Created by liude on 7/10/2017.
//

#ifndef DVRNA_DVRNACORE_H
#define DVRNA_DVRNACORE_H

#include <string>
#include <vector>

#ifdef JNI_OK
#define _ASSERTE(x)
#define nullptr NULL
#endif

// ================ input/result struct ==========================
enum ParamDataType
{
    ParamDataType_Bool,
    ParamDataType_Int,
    ParamDataType_Double,
    ParamDataType_String
};

struct ParamInfo
{
    ParamInfo()
        : strName()
        , eDataType(ParamDataType_Bool)
        , bValue(false)
        , nValue(0)
        , dValue(0.0)
        , strValue()
    {
        ;
    }
    std::string strName;
    ParamDataType eDataType;
    bool bValue;
    int nValue;
    double dValue;
    std::string strValue;
};
typedef std::vector <ParamInfo> StlParamInfoVector;

struct EntryDataInfo
{
    EntryDataInfo()
        : strMethodName()
        , paramVec()
        , retVec()
    {
        ;
    }
    std::string strMethodName;
    StlParamInfoVector paramVec;    // input params.
    StlParamInfoVector retVec;      // return value.
};

// ================ RNA second structure related struct ==========================
#define PI              3.1415926
#define STEM_NUM        4

struct StemInfo
{
    int nStart;
    int nEnd;
    int nPeerStart;
    int nPeerEnd;

    StemInfo()
            : nStart(0)
            , nEnd(0)
            , nPeerStart(0)
            , nPeerEnd(0)
    {
        ;
    }
};

enum AreaType
{
    AreaType_Unknown = -1,
    AreaType_Stem_Acceptor = 0,
    AreaType_Stem_D,
    AreaType_Stem_Anticodon,
    AreaType_Stem_Twc,

    AreaType_Loop_D,
    AreaType_Loop_Anticodon,
    AreaType_Loop_Twc,

    AreaType_Gap_Acceptor_And_D,
    AreaType_Gap_D_And_Anticodon,
    AreaType_Gap_Anticodon_And_Twc
};

struct BaseInfo
{
    char cBase;
    AreaType eAreaType;
    int nPeerIndex;
    int nPosX;
    int nPosY;
    int nBoundKeyValue;

    BaseInfo()
            : cBase('?')
            , eAreaType(AreaType_Unknown)
            , nPeerIndex(-1)
            , nPosX(0)
            , nPosY(0)
            , nBoundKeyValue(-1)
    {
        ;
    }
};
typedef std::vector <BaseInfo> StlBaseInfoVector;

class SecondStructureInfo
{
public:
    SecondStructureInfo();

    bool ValidateInput();
    void FillStemInfos();
    void FillBaseInfos();
    int GetBoundKeyValue(char cA, char cB);
    void ArrangeLayout();
    double Degree2Radian(double dDegree);
    void CalDrawInfo();
    std::string GetResultString();

    // input
    int m_nClientWidth;
    int m_nClientHeight;
    char *m_pSeq;
    char *m_pSS;

    // output
    char m_cError[1024];
    int m_nStemPitch;
    StlBaseInfoVector m_baseInfoVec;
    StemInfo m_stemInfo[4];
};

// ================ methods ==========================
std::string jstring2string(JNIEnv* env, jstring jstr);
void RnaMainEntry(EntryDataInfo &entryDataInfo);
void RnaGetSSDrawInfo(EntryDataInfo &entryDataInfo);

#endif //DVRNA_DVRNACORE_H
