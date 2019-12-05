#include <jni.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "dvrnacore.h"

using namespace std;

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_dv_dvrna_MainActivity_DvRnaEntry(JNIEnv* env, jobject /* this */, jobjectArray inputData)
{
    // 1. parse input parameters.
    EntryDataInfo entryDataInfo;
    int nSize = env->GetArrayLength(inputData);
    if (nSize > 0)
    {
        // the first element is calling method name.
        jobject obj = env->GetObjectArrayElement(inputData, 0);
        jstring jstr = (jstring)obj;
        string strMethodName = jstring2string(env, jstr);
        entryDataInfo.strMethodName = strMethodName;
    }

    int nParamNum = (nSize - 1) / 3;
    for (int nParamIndex = 0; nParamIndex < nParamNum; ++nParamIndex)
    {
        ParamInfo paramInfo;

        // param name.
        jobject obj = env->GetObjectArrayElement(inputData, nParamIndex * 3 + 1);
        jstring jstr = (jstring)obj;
        string strParamName = jstring2string(env, jstr);
        paramInfo.strName = strParamName;

        jobject objDataType = env->GetObjectArrayElement(inputData, nParamIndex * 3 + 2);
        jstring jstrDataType = (jstring)objDataType;
        string strataType = jstring2string(env, jstrDataType);

        jobject objValue = env->GetObjectArrayElement(inputData, nParamIndex * 3 + 3);
        jstring jstrValue = (jstring)objValue;
        string strValue = jstring2string(env, jstrValue);

        if (strataType == string("bool"))
        {
            bool bValue = false;
            if (strValue == string("true"))
                bValue = true;

            paramInfo.eDataType = ParamDataType_Bool;
            paramInfo.bValue = bValue;
        }
        else if (strataType == string("int"))
        {
            int nValue = atoi(strValue.c_str());
            paramInfo.eDataType = ParamDataType_Int;
            paramInfo.nValue = nValue;
        }
        else if (strataType == string("double"))
        {
            double dValue = atof(strValue.c_str());
            paramInfo.eDataType = ParamDataType_Double;
            paramInfo.dValue = dValue;
        }
        else if (strataType == string("string"))
        {
            paramInfo.eDataType = ParamDataType_String;
            paramInfo.strValue = strValue;
        }

        entryDataInfo.paramVec.push_back(paramInfo);
    }

    // 2. call methods.
    RnaMainEntry(entryDataInfo);

    // 3. return result.
    jclass objClass = env->FindClass("java/lang/String");
    jobjectArray resultArray = env->NewObjectArray(entryDataInfo.retVec.size() * 3, objClass, 0);
    for (int nRetIndex = 0; nRetIndex < entryDataInfo.retVec.size(); ++nRetIndex)
    {
        jstring jstrRetName = env->NewStringUTF(entryDataInfo.retVec[nRetIndex].strName.c_str());
        jstring jstrRetDataType, jstrRetValue;
        if (entryDataInfo.retVec[nRetIndex].eDataType == ParamDataType_Bool)
        {
            jstrRetDataType = env->NewStringUTF("bool");
            if (entryDataInfo.retVec[nRetIndex].bValue)
                jstrRetValue = env->NewStringUTF("true");
            else
                jstrRetValue = env->NewStringUTF("false");
        }
        else if (entryDataInfo.retVec[nRetIndex].eDataType == ParamDataType_Int)
        {
            jstrRetDataType = env->NewStringUTF("int");
            char cValue[20] = { 0 };
            sprintf(cValue, "%d", entryDataInfo.retVec[nRetIndex].nValue);
            jstrRetValue = env->NewStringUTF(cValue);
        }
        else if (entryDataInfo.retVec[nRetIndex].eDataType == ParamDataType_Double)
        {
            jstrRetDataType = env->NewStringUTF("double");
            char cValue[20] = { 0 };
            sprintf(cValue, "%.3f", entryDataInfo.retVec[nRetIndex].dValue);
            jstrRetValue = env->NewStringUTF(cValue);
        }
        else if (entryDataInfo.retVec[nRetIndex].eDataType == ParamDataType_String)
        {
            jstrRetDataType = env->NewStringUTF("string");
            jstrRetValue = env->NewStringUTF(entryDataInfo.retVec[nRetIndex].strValue.c_str());
        }

        env->SetObjectArrayElement(resultArray, nRetIndex * 3 + 0, jstrRetName);
        env->SetObjectArrayElement(resultArray, nRetIndex * 3 + 1, jstrRetDataType);
        env->SetObjectArrayElement(resultArray, nRetIndex * 3 + 2, jstrRetValue);
    }

    return resultArray;
}

std::string jstring2string(JNIEnv* env, jstring jstr)
{
    std::string strRet("");

    char *rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("GB2312");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray)env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0)
    {
        rtn = (char*)malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;

        std::string stemp(rtn);
        free(rtn);
        strRet = stemp;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    return strRet;
}

void RnaMainEntry(EntryDataInfo &entryDataInfo)
{
    if (entryDataInfo.strMethodName == string("RnaGetSSDrawInfo"))
    {
        RnaGetSSDrawInfo(entryDataInfo);
    }
}

// ================ RNA second structure related methods ==========================
void RnaGetSSDrawInfo(EntryDataInfo &entryDataInfo)
{
    SecondStructureInfo ssInfo;
    ssInfo.m_pSeq = (char *)entryDataInfo.paramVec[0].strValue.c_str();
    ssInfo.m_pSS = (char *)entryDataInfo.paramVec[1].strValue.c_str();
    ssInfo.m_nClientWidth = entryDataInfo.paramVec[2].nValue;
    ssInfo.m_nClientHeight = entryDataInfo.paramVec[3].nValue;

    ssInfo.CalDrawInfo();

    ParamInfo piReturn;
    piReturn.strName = "Return";
    piReturn.eDataType = ParamDataType_String;
    piReturn.strValue = std::string(ssInfo.m_cError);
    entryDataInfo.retVec.push_back(piReturn);

    ParamInfo piResult;
    piResult.strName = "Result";
    piResult.eDataType = ParamDataType_String;
    if (strlen(ssInfo.m_cError) == 0)
    {
        piResult.strValue = ssInfo.GetResultString();
    }
    entryDataInfo.retVec.push_back(piResult);
}

SecondStructureInfo::SecondStructureInfo()
    : m_nClientWidth(600)
    , m_nClientHeight(800)
    , m_pSeq(nullptr)
    , m_pSS(nullptr)
    , m_nStemPitch(10)
    , m_baseInfoVec()
{
    m_cError[0] = '\0';
}

bool SecondStructureInfo::ValidateInput()
{
    // checking input.
    if (strlen(m_pSeq) < 50)
    {
        strcpy(m_cError, "The data length for tRNA sequence is tool small!");
        return false;
    }

    if (strlen(m_pSS) < 50)
    {
        strcpy(m_cError, "The data length for tRNA secondary structure is tool small!");
        return false;
    }

    if (strlen(m_pSeq) != strlen(m_pSS))
    {
        strcpy(m_cError, "The data length for tRNA sequence and secondary structure is not equal!");
        return false;
    }

    int nSSLeftBracket = 0;
    int nSSRightBracket = 0;
    for (int i = 0; i < strlen(m_pSeq); ++i)
    {
        if (m_pSeq[i] == 'A' || m_pSeq[i] == 'T' || m_pSeq[i] == 'G' || m_pSeq[i] == 'C')
        {
            ;
        }
        else if (m_pSeq[i] == 'a')
            m_pSeq[i] = 'A';
        else if (m_pSeq[i] == 'c')
            m_pSeq[i] = 'C';
        else if (m_pSeq[i] == 'g')
            m_pSeq[i] = 'G';
        else if (m_pSeq[i] == 't' || m_pSeq[i] == 'u' || m_pSeq[i] == 'U')
            m_pSeq[i] = 'T';
        else
        {
            sprintf(m_cError, "Unrecognized base found at position [%d] which is [%c]!", i + 1, m_pSeq[i]);
            return false;
        }

        if (m_pSS[i] == '<' || m_pSS[i] == '>' || m_pSS[i] == '.')
        {
            if (m_pSS[i] == '<')
                nSSLeftBracket++;
            else if (m_pSS[i] == '>')
                nSSRightBracket++;
        }
        else
        {
            sprintf(m_cError, "Unrecognized secondary structure annotation character found \nat position [%d] which is [%c]!\nOnly <, >, and . is allowed.", i + 1, m_pSS[i]);
            return false;
        }
    }

    if (nSSLeftBracket == 0 || nSSRightBracket == 0)
    {
        strcpy(m_cError, "No stem annotation found in secondary structure data!");
        return false;
    }
    else if (nSSLeftBracket != nSSRightBracket)
    {
        strcpy(m_cError, "Stem annotation for secondary structure is wrong!\nthe number of < and > is not matched!");
        return false;
    }

    return true;
}

void SecondStructureInfo::FillStemInfos()
{
    char *pSearchBegin = m_pSS;
    char *pB = nullptr;
    char *pA = nullptr;
    int nStart = 0;
    int nEnd = 0;
    int nPeerStart = 0;
    int nPeerEnd = 0;

    for (int nStemIndex = 1; nStemIndex <= 3; ++nStemIndex)
    {
        // find the first reverse <.
        pB = strchr(pSearchBegin, '<');
        pA = pB - 1;
        while (*pA == '.')
            pA--;

        nEnd = pA - m_pSS;
        nPeerStart = pB - m_pSS;

        while (*pB != '.' && *pA != '.')
        {
            pB++;
            pA--;
        }

        nStart = pA - m_pSS + 1;
        nPeerEnd = pB - m_pSS - 1;

        m_stemInfo[nStemIndex].nStart = nStart;
        m_stemInfo[nStemIndex].nPeerEnd = nPeerEnd;
        m_stemInfo[nStemIndex].nEnd = nEnd;
        m_stemInfo[nStemIndex].nPeerStart = nPeerStart;

        pSearchBegin = m_pSS + m_stemInfo[nStemIndex].nPeerEnd + 1;
    }

    // accepter stem.
    pB = m_pSS + m_stemInfo[AreaType_Stem_Twc].nPeerEnd + 1;
    pA = m_pSS + m_stemInfo[AreaType_Stem_D].nStart - 1;
    while (*pB == '.')
        pB++;
    while (*pA == '.')
        pA--;

    nEnd = pA - m_pSS;
    nPeerStart = pB - m_pSS;

    _ASSERTE(m_pSS[0] == '>');
    _ASSERTE(m_pSS[strlen(m_pSS) - 2] == '<');

    nStart = 0;
    nPeerEnd = strlen(m_pSS) - 2;

    _ASSERTE((nEnd - nStart) == (nPeerEnd - nPeerStart));
    m_stemInfo[AreaType_Stem_Acceptor].nStart = nStart;
    m_stemInfo[AreaType_Stem_Acceptor].nPeerEnd = nPeerEnd;
    m_stemInfo[AreaType_Stem_Acceptor].nEnd = nEnd;
    m_stemInfo[AreaType_Stem_Acceptor].nPeerStart = nPeerStart;
}

int SecondStructureInfo::GetBoundKeyValue(char cA, char cB)
{
    int nBondLevel = 0;
    if ((cA == 'G' && cB == 'T') || (cA == 'T' && cB == 'G'))
        nBondLevel = 1;
    else if ((cA == 'A' && cB == 'T') || (cA == 'T' && cB == 'A'))
        nBondLevel = 2;
    else if ((cA == 'G' && cB == 'C') || (cA == 'C' && cB == 'G'))
        nBondLevel = 3;

    return nBondLevel;
}

void SecondStructureInfo::FillBaseInfos()
{
    // get stems info.
    int nLen = strlen(m_pSeq);
    for (int i = 0; i < nLen; ++i)
    {
        BaseInfo bi;
        bi.cBase = m_pSeq[i];

        for (int j = 0; j < STEM_NUM; ++j)
        {
            if (i >= m_stemInfo[j].nStart && i <= m_stemInfo[j].nEnd)
            {
                bi.nPeerIndex = m_stemInfo[j].nPeerStart + (m_stemInfo[j].nEnd - i);
                break;
            }

            if (bi.nPeerIndex != -1)
                break;
        }

        if (bi.nPeerIndex != -1)
        {
            bi.nBoundKeyValue = GetBoundKeyValue(bi.cBase, m_pSeq[bi.nPeerIndex]);
        }

        m_baseInfoVec.push_back(bi);
    }

    int nStartIndex = m_stemInfo[AreaType_Stem_Acceptor].nStart;
    int nEndIndex = m_stemInfo[AreaType_Stem_Acceptor].nEnd;
    for (int i = nStartIndex; i <= nEndIndex; ++i)
    {
        m_baseInfoVec[i].eAreaType = AreaType_Stem_Acceptor;
    }

    nStartIndex = m_stemInfo[AreaType_Stem_Acceptor].nEnd + 1;
    nEndIndex = m_stemInfo[AreaType_Stem_D].nStart - 1;
    for (int i = nStartIndex; i <= nEndIndex; ++i)
    {
        m_baseInfoVec[i].eAreaType = AreaType_Gap_Acceptor_And_D;
    }

    nStartIndex = m_stemInfo[AreaType_Stem_D].nStart;
    nEndIndex = m_stemInfo[AreaType_Stem_D].nEnd;
    for (int i = nStartIndex; i <= nEndIndex; ++i)
    {
        m_baseInfoVec[i].eAreaType = AreaType_Stem_D;
    }

    nStartIndex = m_stemInfo[AreaType_Stem_D].nEnd + 1;
    nEndIndex = m_stemInfo[AreaType_Stem_D].nPeerStart - 1;
    for (int i = nStartIndex; i <= nEndIndex; ++i)
    {
        m_baseInfoVec[i].eAreaType = AreaType_Loop_D;
    }

    nStartIndex = m_stemInfo[AreaType_Stem_D].nPeerStart;
    nEndIndex = m_stemInfo[AreaType_Stem_D].nPeerEnd;
    for (int i = nStartIndex; i <= nEndIndex; ++i)
    {
        m_baseInfoVec[i].eAreaType = AreaType_Stem_D;
    }

    nStartIndex = m_stemInfo[AreaType_Stem_D].nPeerEnd + 1;
    nEndIndex = m_stemInfo[AreaType_Stem_Anticodon].nStart - 1;
    for (int i = nStartIndex; i <= nEndIndex; ++i)
    {
        m_baseInfoVec[i].eAreaType = AreaType_Gap_D_And_Anticodon;
    }

    nStartIndex = m_stemInfo[AreaType_Stem_Anticodon].nStart;
    nEndIndex = m_stemInfo[AreaType_Stem_Anticodon].nEnd;
    for (int i = nStartIndex; i <= nEndIndex; ++i)
    {
        m_baseInfoVec[i].eAreaType = AreaType_Stem_Anticodon;
    }

    nStartIndex = m_stemInfo[AreaType_Stem_Anticodon].nEnd + 1;
    nEndIndex = m_stemInfo[AreaType_Stem_Anticodon].nPeerStart - 1;
    for (int i = nStartIndex; i <= nEndIndex; ++i)
    {
        m_baseInfoVec[i].eAreaType = AreaType_Loop_Anticodon;
    }

    nStartIndex = m_stemInfo[AreaType_Stem_Anticodon].nPeerStart;
    nEndIndex = m_stemInfo[AreaType_Stem_Anticodon].nPeerEnd;
    for (int i = nStartIndex; i <= nEndIndex; ++i)
    {
        m_baseInfoVec[i].eAreaType = AreaType_Stem_Anticodon;
    }

    nStartIndex = m_stemInfo[AreaType_Stem_Anticodon].nPeerEnd + 1;
    nEndIndex = m_stemInfo[AreaType_Stem_Twc].nStart - 1;
    for (int i = nStartIndex; i <= nEndIndex; ++i)
    {
        m_baseInfoVec[i].eAreaType = AreaType_Gap_Anticodon_And_Twc;
    }

    nStartIndex = m_stemInfo[AreaType_Stem_Twc].nStart;
    nEndIndex = m_stemInfo[AreaType_Stem_Twc].nEnd;
    for (int i = nStartIndex; i <= nEndIndex; ++i)
    {
        m_baseInfoVec[i].eAreaType = AreaType_Stem_Twc;
    }

    nStartIndex = m_stemInfo[AreaType_Stem_Twc].nEnd + 1;
    nEndIndex = m_stemInfo[AreaType_Stem_Twc].nPeerStart - 1;
    for (int i = nStartIndex; i <= nEndIndex; ++i)
    {
        m_baseInfoVec[i].eAreaType = AreaType_Loop_Twc;
    }

    nStartIndex = m_stemInfo[AreaType_Stem_Twc].nPeerStart;
    nEndIndex = m_stemInfo[AreaType_Stem_Twc].nPeerEnd;
    for (int i = nStartIndex; i <= nEndIndex; ++i)
    {
        m_baseInfoVec[i].eAreaType = AreaType_Stem_Twc;
    }

    nStartIndex = m_stemInfo[AreaType_Stem_Twc].nPeerEnd + 1;
    nEndIndex = nLen - 1;
    for (int i = nStartIndex; i <= nEndIndex; ++i)
    {
        m_baseInfoVec[i].eAreaType = AreaType_Stem_Acceptor;
    }
}

void SecondStructureInfo::ArrangeLayout()
{
    // get bounding box.
    int nBoundingBoxLeft = 0;
    int nBoundingBoxTop = 0;
    int nBoundingBoxW = 0;
    int nBoundingBoxH = 0;
    {
        int l = 999;
        int r = -999;
        int t = 999;
        int b = 0;
        for (int i = 0; i < m_baseInfoVec.size(); ++i)
        {
            BaseInfo &bi = m_baseInfoVec[i];

            if (bi.nPosX < l)
                l = bi.nPosX;

            if (bi.nPosX > r)
                r = bi.nPosX;

            if (bi.nPosY < t)
                t = bi.nPosY;

            if (bi.nPosY > b)
                b = bi.nPosY;
        }

        nBoundingBoxLeft = l;
        nBoundingBoxTop = t;
        nBoundingBoxW = r - l;
        nBoundingBoxH = b - t;
    }

    // fix to screen
    double dRatioSS = nBoundingBoxW * 1.0 / nBoundingBoxH;
    double dRatioScreen = m_nClientWidth * 1.0 / m_nClientHeight;

    int nDrawScreenW = m_nClientWidth;
    int nDrawScreenH = m_nClientHeight;

    if (dRatioSS > dRatioScreen)
    {
        nDrawScreenH = (int)(m_nClientWidth / dRatioSS);
    }
    else
    {
        nDrawScreenW = (int)(m_nClientHeight * dRatioScreen);
    }

    // scale
    double dScale = nDrawScreenW * 1.0 / nBoundingBoxW;
    for (int i = 0; i < m_baseInfoVec.size(); ++i)
    {
        BaseInfo &bi = m_baseInfoVec[i];

        bi.nPosX = (int)(bi.nPosX * dScale);
        bi.nPosY = (int)(bi.nPosY * dScale);
    }

    // move to fit pos.
    int nOffsetX = (int)(m_nClientWidth * 0.5);
    int nOffsetY = 0;
    if (dRatioSS > dRatioScreen)
        nOffsetY = (int)((m_nClientHeight - nDrawScreenH) * 0.5);

    for (int i = 0; i < m_baseInfoVec.size(); ++i)
    {
        BaseInfo &bi = m_baseInfoVec[i];

        bi.nPosX += nOffsetX;
        bi.nPosY += nOffsetY;
    }
}

double SecondStructureInfo::Degree2Radian(double dDegree)
{
    return (dDegree * PI / 180);
}

void SecondStructureInfo::CalDrawInfo()
{
    // validate the input
    if (!ValidateInput())
        return;

    // fill base info.
    FillStemInfos();
    FillBaseInfos();

    // calculate each base pos.
    // acceptor stem.
    for (int i = m_stemInfo[AreaType_Stem_Acceptor].nStart; i <= m_stemInfo[AreaType_Stem_Acceptor].nEnd; ++i)
    {
        m_baseInfoVec[i].nPosX = 0;
        m_baseInfoVec[i].nPosY = i * m_nStemPitch;

        // peer.
        int nPeerIndex = m_baseInfoVec[i].nPeerIndex;
        _ASSERTE(nPeerIndex != -1);
        m_baseInfoVec[nPeerIndex].nPosX = m_nStemPitch;
        m_baseInfoVec[nPeerIndex].nPosY = m_baseInfoVec[i].nPosY;
    }

    _ASSERTE(m_stemInfo[AreaType_Stem_Acceptor].nStart == 0);
    int nAmioAcidLen = m_baseInfoVec.size() - m_stemInfo[AreaType_Stem_Acceptor].nPeerEnd - 1;
    for (int i = 1; i <= nAmioAcidLen; ++i)
    {
        int nPeerIndex = m_stemInfo[AreaType_Stem_Acceptor].nPeerEnd + i;
        m_baseInfoVec[nPeerIndex].nPosX = m_baseInfoVec[m_stemInfo[AreaType_Stem_Acceptor].nPeerEnd].nPosX;
        m_baseInfoVec[nPeerIndex].nPosY = m_baseInfoVec[m_stemInfo[AreaType_Stem_Acceptor].nPeerEnd].nPosY - (m_nStemPitch * i);
    }

    // gap between acceptor stem & D-Stem.
    {
        int nStartLoopIndex = m_stemInfo[AreaType_Stem_Acceptor].nEnd;
        int nEndLoopIndex = m_stemInfo[AreaType_Stem_D].nStart;
        int nEdgeNum = nEndLoopIndex - nStartLoopIndex;

        double dLoopPitchAngle = Degree2Radian(90.0) / nEdgeNum;
        double dLoopRadius = (m_nStemPitch * 0.5) / sin(dLoopPitchAngle * 0.5);
        double dLoopCenterX = m_baseInfoVec[nStartLoopIndex].nPosX - dLoopRadius;
        double dLoopCenterY = m_baseInfoVec[nStartLoopIndex].nPosY;

        for (int i = 1; i <= nEdgeNum; ++i)
        {
            double dDeltaAngle = i * dLoopPitchAngle;

            double dOffsetX = dLoopRadius * cos(dDeltaAngle);
            double dOffsetY = dLoopRadius * sin(dDeltaAngle);

            int nNextIndex = nStartLoopIndex + i;
            m_baseInfoVec[nNextIndex].nPosX = (int)(dLoopCenterX + dOffsetX);
            m_baseInfoVec[nNextIndex].nPosY = (int)(dLoopCenterY + dOffsetY);
        }
    }

    // D-stem
    for (int i = m_stemInfo[AreaType_Stem_D].nStart; i <= m_stemInfo[AreaType_Stem_D].nEnd; ++i)
    {
        // the first one has been calculated in above.
        if (i > m_stemInfo[AreaType_Stem_D].nStart)
        {
            m_baseInfoVec[i].nPosX = m_baseInfoVec[i - 1].nPosX - m_nStemPitch;
            m_baseInfoVec[i].nPosY = m_baseInfoVec[i - 1].nPosY;
        }

        // peer.
        int nPeerIndex = m_baseInfoVec[i].nPeerIndex;
        _ASSERTE(nPeerIndex != -1);
        m_baseInfoVec[nPeerIndex].nPosX = m_baseInfoVec[i].nPosX;
        m_baseInfoVec[nPeerIndex].nPosY = m_baseInfoVec[i].nPosY + m_nStemPitch;
    }

    // D-Loop.
    {
        int nStartLoopIndex = m_stemInfo[AreaType_Stem_D].nEnd;
        int nEndLoopIndex = m_baseInfoVec[nStartLoopIndex].nPeerIndex;
        _ASSERTE(nEndLoopIndex != -1);

        int nEdgeNum = nEndLoopIndex - nStartLoopIndex + 1;
        _ASSERTE(nStartLoopIndex > 0);

        double dLoopPitchAngle = Degree2Radian(360.0) / nEdgeNum;
        double dLoopRadius = (m_nStemPitch * 0.5) / sin(dLoopPitchAngle * 0.5);
        double dLoopCenterX = m_baseInfoVec[nStartLoopIndex].nPosX - (dLoopRadius * cos(dLoopPitchAngle * 0.5));
        double dLoopCenterY = m_baseInfoVec[nStartLoopIndex].nPosY + m_nStemPitch * 0.5;

        for (int i = 1; i < nEdgeNum - 1; ++i)
        {
            double dDeltaAngle = i * dLoopPitchAngle + dLoopPitchAngle * 0.5;

            double dOffsetX = dLoopRadius * cos(dDeltaAngle);
            double dOffsetY = dLoopRadius * sin(dDeltaAngle) * (-1);

            int nNextIndex = nStartLoopIndex + i;
            m_baseInfoVec[nNextIndex].nPosX = (int)(dLoopCenterX + dOffsetX);
            m_baseInfoVec[nNextIndex].nPosY = (int)(dLoopCenterY + dOffsetY);
        }
    }

    // gap between D-stem & anticoden stem.
    {
        int nStartLoopIndex = m_stemInfo[AreaType_Stem_D].nPeerEnd;
        int nEndLoopIndex = m_stemInfo[AreaType_Stem_Anticodon].nStart;
        int nEdgeNum = nEndLoopIndex - nStartLoopIndex;

        double dLoopPitchAngle = Degree2Radian(90.0) / nEdgeNum;
        double dLoopRadius = (m_nStemPitch * 0.5) / sin(dLoopPitchAngle * 0.5);
        double dLoopCenterX = m_baseInfoVec[nStartLoopIndex].nPosX;
        double dLoopCenterY = m_baseInfoVec[nStartLoopIndex].nPosY + dLoopRadius;

        for (int i = 1; i <= nEdgeNum; ++i)
        {
            double dDeltaAngle = i * dLoopPitchAngle;

            double dOffsetX = dLoopRadius * sin(dDeltaAngle);
            double dOffsetY = dLoopRadius * cos(dDeltaAngle) * (-1);

            int nNextIndex = nStartLoopIndex + i;
            m_baseInfoVec[nNextIndex].nPosX = (int)(dLoopCenterX + dOffsetX);
            m_baseInfoVec[nNextIndex].nPosY = (int)(dLoopCenterY + dOffsetY);
        }
    }

    // anticoden-stem
    for (int i = m_stemInfo[AreaType_Stem_Anticodon].nStart; i <= m_stemInfo[AreaType_Stem_Anticodon].nEnd; ++i)
    {
        // the first one has been calculated in above.
        if (i > m_stemInfo[AreaType_Stem_Anticodon].nStart)
        {
            m_baseInfoVec[i].nPosX = m_baseInfoVec[i - 1].nPosX;
            m_baseInfoVec[i].nPosY = m_baseInfoVec[i - 1].nPosY + m_nStemPitch;
        }

        // peer.
        int nPeerIndex = m_baseInfoVec[i].nPeerIndex;
        _ASSERTE(nPeerIndex != -1);
        m_baseInfoVec[nPeerIndex].nPosX = m_baseInfoVec[i].nPosX + m_nStemPitch;
        m_baseInfoVec[nPeerIndex].nPosY = m_baseInfoVec[i].nPosY;
    }

    // anticoden-Loop.
    {
        int nStartLoopIndex = m_stemInfo[AreaType_Stem_Anticodon].nEnd;
        int nEndLoopIndex = m_baseInfoVec[nStartLoopIndex].nPeerIndex;
        _ASSERTE(nEndLoopIndex != -1);

        int nEdgeNum = nEndLoopIndex - nStartLoopIndex + 1;
        _ASSERTE(nStartLoopIndex > 0);

        double dLoopPitchAngle = Degree2Radian(360.0) / nEdgeNum;
        double dLoopRadius = (m_nStemPitch * 0.5) / sin(dLoopPitchAngle * 0.5);
        double dLoopCenterX = m_baseInfoVec[nStartLoopIndex].nPosX + m_nStemPitch * 0.5;
        double dLoopCenterY = m_baseInfoVec[nStartLoopIndex].nPosY + (dLoopRadius * cos(dLoopPitchAngle * 0.5));

        for (int i = 1; i < nEdgeNum - 1; ++i)
        {
            double dDeltaAngle = i * dLoopPitchAngle + dLoopPitchAngle * 0.5;

            double dOffsetX = dLoopRadius * sin(dDeltaAngle) * (-1);
            double dOffsetY = dLoopRadius * cos(dDeltaAngle) * (-1);

            int nNextIndex = nStartLoopIndex + i;
            m_baseInfoVec[nNextIndex].nPosX = (int)(dLoopCenterX + dOffsetX);
            m_baseInfoVec[nNextIndex].nPosY = (int)(dLoopCenterY + dOffsetY);
        }
    }

    // gap between acceptor stem & twc-Stem.
    {
        int nStartLoopIndex = m_stemInfo[AreaType_Stem_Twc].nPeerEnd;
        int nEndLoopIndex = m_stemInfo[AreaType_Stem_Acceptor].nPeerStart;
        int nEdgeNum = nEndLoopIndex - nStartLoopIndex;
        _ASSERTE(nEdgeNum == 1);

        double dLoopPitchAngle = Degree2Radian(90.0) / nEdgeNum;
        double dLoopRadius = (m_nStemPitch * 0.5) / sin(dLoopPitchAngle * 0.5);
        double dLoopCenterX = m_baseInfoVec[nEndLoopIndex].nPosX + dLoopRadius;
        double dLoopCenterY = m_baseInfoVec[nEndLoopIndex].nPosY;

        for (int i = 1; i <= nEdgeNum; ++i)
        {
            double dDeltaAngle = i * dLoopPitchAngle;

            double dOffsetX = dLoopRadius * cos(dDeltaAngle) * (-1);
            double dOffsetY = dLoopRadius * sin(dDeltaAngle);

            int nNextIndex = nEndLoopIndex - i;
            m_baseInfoVec[nNextIndex].nPosX = (int)(dLoopCenterX + dOffsetX);
            m_baseInfoVec[nNextIndex].nPosY = (int)(dLoopCenterY + dOffsetY);

            if (i == nEdgeNum)
            {
                int nNextPeerIndex = m_stemInfo[AreaType_Stem_Twc].nStart;
                m_baseInfoVec[nNextPeerIndex].nPosX = m_baseInfoVec[nNextIndex].nPosX;
                m_baseInfoVec[nNextPeerIndex].nPosY = m_baseInfoVec[nNextIndex].nPosY + m_nStemPitch;
            }
        }
    }

    // twc-stem
    for (int i = m_stemInfo[AreaType_Stem_Twc].nStart; i <= m_stemInfo[AreaType_Stem_Twc].nEnd; ++i)
    {
        // the first one has been calculated in above.
        if (i > m_stemInfo[AreaType_Stem_Twc].nStart)
        {
            m_baseInfoVec[i].nPosX = m_baseInfoVec[i - 1].nPosX + m_nStemPitch;
            m_baseInfoVec[i].nPosY = m_baseInfoVec[i - 1].nPosY;
        }

        // peer.
        int nPeerIndex = m_baseInfoVec[i].nPeerIndex;
        _ASSERTE(nPeerIndex != -1);
        m_baseInfoVec[nPeerIndex].nPosX = m_baseInfoVec[i].nPosX;
        m_baseInfoVec[nPeerIndex].nPosY = m_baseInfoVec[i].nPosY - m_nStemPitch;
    }

    // twc-Loop.
    {
        int nStartLoopIndex = m_stemInfo[AreaType_Stem_Twc].nEnd;
        int nEndLoopIndex = m_baseInfoVec[nStartLoopIndex].nPeerIndex;
        _ASSERTE(nEndLoopIndex != -1);

        int nEdgeNum = nEndLoopIndex - nStartLoopIndex + 1;
        _ASSERTE(nStartLoopIndex > 0);

        double dLoopPitchAngle = Degree2Radian(360.0) / nEdgeNum;
        double dLoopRadius = (m_nStemPitch * 0.5) / sin(dLoopPitchAngle * 0.5);
        double dLoopCenterX = m_baseInfoVec[nStartLoopIndex].nPosX + (dLoopRadius * cos(dLoopPitchAngle * 0.5));
        double dLoopCenterY = m_baseInfoVec[nStartLoopIndex].nPosY - m_nStemPitch * 0.5;

        for (int i = 1; i < nEdgeNum - 1; ++i)
        {
            double dDeltaAngle = i * dLoopPitchAngle + dLoopPitchAngle * 0.5;

            double dOffsetX = dLoopRadius * cos(dDeltaAngle) * (-1);
            double dOffsetY = dLoopRadius * sin(dDeltaAngle);

            int nNextIndex = nStartLoopIndex + i;
            m_baseInfoVec[nNextIndex].nPosX = (int)(dLoopCenterX + dOffsetX);
            m_baseInfoVec[nNextIndex].nPosY = (int)(dLoopCenterY + dOffsetY);
        }
    }

    // gap between anticoden stem & twc-Stem.
    {
        int nStartLoopIndex = m_stemInfo[AreaType_Stem_Anticodon].nPeerEnd;
        int nEndLoopIndex = m_stemInfo[AreaType_Stem_Twc].nStart;
        int nEdgeNum = nEndLoopIndex - nStartLoopIndex - 1;

        double dA = abs(m_baseInfoVec[nEndLoopIndex].nPosX - m_baseInfoVec[nStartLoopIndex].nPosX);
        double dB = abs(m_baseInfoVec[nEndLoopIndex].nPosY - m_baseInfoVec[nStartLoopIndex].nPosY);
        double dC = sqrt(dA * dA + dB * dB);

        double dAngle0 = acos(dA / dC);
        double dAngle1 = PI - dAngle0 - Degree2Radian(45.0);

        double dD = dC * cos(dAngle1);

        int nDArmNum = (int)(dD / m_nStemPitch + 1);

        double dRadius = dC * sin(dAngle1) / 2;
        double dAngle2 = asin(m_nStemPitch * 0.5 / dRadius) * 2;

        int nLoopNum = (int)(PI / dAngle2 + 1);

        dAngle2 = PI / nLoopNum;

        nLoopNum -= 2;

        int nRemain = nEdgeNum - nDArmNum - nLoopNum;

        int nArmNum = 0;
        if (nRemain < 2)
        {
            nLoopNum = nEdgeNum - nDArmNum;
        }
        else
        {
            if (nRemain % 2 == 1)
            {
                nLoopNum++;
                nRemain--;
            }

            nArmNum = nRemain / 2;
        }

        // for arm D.
        for (int i = 1; i <= nDArmNum; ++i)
        {
            int nNextIndex = nEndLoopIndex - i;

            double dOffsetX = dD / nDArmNum * cos(Degree2Radian(45.0));
            double dOffsetY = dD / nDArmNum * sin(Degree2Radian(45.0));

            m_baseInfoVec[nNextIndex].nPosX = (int)(m_baseInfoVec[nNextIndex + 1].nPosX + dOffsetX);
            m_baseInfoVec[nNextIndex].nPosY = (int)(m_baseInfoVec[nNextIndex + 1].nPosY + dOffsetY);
        }

        // for arm up.
        for (int i = 1; i <= nArmNum; ++i)
        {
            int nNextIndex = nEndLoopIndex - nDArmNum - i;

            double dOffsetX = m_nStemPitch * cos(Degree2Radian(45.0));
            double dOffsetY = m_nStemPitch * sin(Degree2Radian(45.0));

            m_baseInfoVec[nNextIndex].nPosX = (int)(m_baseInfoVec[nNextIndex + 1].nPosX + dOffsetX);
            m_baseInfoVec[nNextIndex].nPosY = (int)(m_baseInfoVec[nNextIndex + 1].nPosY + dOffsetY);
        }

        // for arm bottom.
        for (int i = 1; i <= nArmNum; ++i)
        {
            int nNextIndex = nStartLoopIndex + i;

            double dOffsetX = m_nStemPitch * cos(Degree2Radian(45.0));
            double dOffsetY = m_nStemPitch * sin(Degree2Radian(45.0));

            m_baseInfoVec[nNextIndex].nPosX = (int)(m_baseInfoVec[nNextIndex + 1].nPosX + dOffsetX);
            m_baseInfoVec[nNextIndex].nPosY = (int)(m_baseInfoVec[nNextIndex + 1].nPosY + dOffsetY);
        }

        // for loop.
        double dCenterX = (m_baseInfoVec[nEndLoopIndex - nDArmNum - nArmNum].nPosX + m_baseInfoVec[nStartLoopIndex + nArmNum].nPosX) * 0.5;
        double dCenterY = (m_baseInfoVec[nEndLoopIndex - nDArmNum - nArmNum].nPosY + m_baseInfoVec[nStartLoopIndex + nArmNum].nPosY) * 0.5;

        for (int i = 1; i <= nLoopNum; ++i)
        {
            int nNextIndex = nEndLoopIndex - nDArmNum - nArmNum - i;

            double dDeltaAngle = Degree2Radian(180.0) / (nLoopNum + 1);
            double dCurrentAngle = dDeltaAngle * i;
            double dRealAngle = Degree2Radian(45.0) - dCurrentAngle;

            double dOffsetX = dRadius * cos(dRealAngle);
            double dOffsetY = dRadius * sin(dRealAngle);

            m_baseInfoVec[nNextIndex].nPosX = (int)(dCenterX + dOffsetX);
            m_baseInfoVec[nNextIndex].nPosY = (int)(dCenterY - dOffsetY);
        }
    }

    ArrangeLayout();
}

std::string SecondStructureInfo::GetResultString()
{
    std::string strRet;
    for (int i = 0; i < m_baseInfoVec.size(); ++i)
    {
        BaseInfo &bi = m_baseInfoVec[i];
        char cBaseInfo[512] = { 0 };
        sprintf(cBaseInfo, "%c,%d,%d,%d,%d,%d", bi.cBase, bi.eAreaType, bi.nPeerIndex, bi.nPosX, bi.nPosY, bi.nBoundKeyValue);
        std::string strLine(cBaseInfo);
        strRet += strLine;
        if (i != m_baseInfoVec.size() - 1)
            strRet += ";";
    }

    return strRet;
}
