package com.dv.dvrna;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.LinearGradient;
import android.graphics.Paint;
import android.graphics.Point;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;
import java.util.ArrayList;

public class tRNASecondaryView extends View
{
    private static final int AreaType_Stem_Acceptor = 0;
    private static final int AreaType_Stem_D = 1;
    private static final int AreaType_Stem_Anticodon = 2;
    private static final int AreaType_Stem_Twc = 3;
    private static final int AreaType_Loop_D = 4;
    private static final int AreaType_Loop_Anticodon = 5;
    private static final int AreaType_Loop_Twc = 6;
    private static final int AreaType_Gap_Acceptor_And_D = 7;
    private static final int AreaType_Gap_D_And_Anticodon = 8;
    private static final int AreaType_Gap_Anticodon_And_Twc = 9;

    private class BaseInfo
    {
        String strBase;
        int nAreaType;
        int nPeerIndex;
        int nPosX;
        int nPosY;
        int nBoundKeyValue;
    };

    private Paint m_paint = null;
    private Rect m_rtClient = new Rect();   // drawing rect.
    private int m_nMargin = 50;  // drawing margin
    private ArrayList<BaseInfo> m_baseInfoList = null;
    private String m_strMsg =
            "This tool is used to draw tRNA by giving tRNA sequence and \n" +
            "secondary structure data.\n" +
            "Usage:\n" +
            "    1. Access tRNAscan-SE online tool to get secondary structure data.\n" +
            "       http://lowelab.ucsc.edu/tRNAscan-SE/\n" +
            "       After you have got sequence & structure data, then back to this tool.\n" +
            "    2. Click the INPUT button on top bar.\n" +
            "       In the popup dialog, copy/paste the sequence and structure data.\n" +
            "       You can click GET SAMPLE DATA button to see the sample data.\n" +
            "       Click OK button.\n" +
            "    3. The tRAN structure will be drown in the view if no error occurs.\n" +
            "       Or error message will be shown\n" +
            "If you have any comments you can send mail to dfliu@163.com";

    // methods.
    private void MyLog(String strMsg)
    {
        strMsg = "[tRNASecondaryView] " + strMsg;
        Log.i("MyDebug", strMsg);
    }

    public tRNASecondaryView(Context context, AttributeSet attrs)
    {
        super(context, attrs);

        MyLog("tRNASecondaryView()->");
        m_paint = new Paint(Paint.ANTI_ALIAS_FLAG);
        MyLog("tRNASecondaryView()<-");
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh)
    {
        super.onSizeChanged(w, h, oldw, oldh);

        MyLog("onSizeChanged()->");

        m_rtClient.left = getLeft() + m_nMargin;
        m_rtClient.right = getRight() - m_nMargin;
        m_rtClient.top = getTop() + m_nMargin;
        m_rtClient.bottom = getBottom() - m_nMargin;

//        int[] location = new int[2];
//        getLocationOnScreen(location);
//        MyLog("onDraw() getLocationOnScreen:" + String.valueOf(location[0]) + " " + String.valueOf(location[1]));
//
//        int[] location1 = new int[2];
//        getLocationInWindow(location1);
//        MyLog("onDraw() getLocationInWindow:" + String.valueOf(location1[0]) + " " + String.valueOf(location1[1]));

//        m_rtClient.left = getLeft();
//        m_rtClient.right = getRight();
//        m_rtClient.top = location1[1];
//        m_rtClient.bottom = location1[1] + getBottom();

        MyLog("onDraw() m_rtClient:" + m_rtClient.toString());

        MyLog("onSizeChanged()<-");
    }

    public int GetClientWidth()
    {
        return m_rtClient.width();
    }

    public int GetClientHeight()
    {
        return m_rtClient.height();
    }

    public void ShowMsg(String strMsg)
    {
        MyLog("ShowMsg()->");
        m_strMsg = strMsg;
        invalidate();
        MyLog("ShowMsg()<-");
    }

    public void DrawSS(String strSSInfo)
    {
        MyLog("DrawSS()->");
        m_strMsg = null;

        m_baseInfoList = new ArrayList<BaseInfo>();
        String[] baseInfos = strSSInfo.split(";");

        for (int i = 0; i < baseInfos.length; ++i)
        {
            String[] fields = baseInfos[i].split(",");

            BaseInfo bi = new BaseInfo();
            bi.strBase = fields[0];
            bi.nAreaType = Integer.valueOf(fields[1]);
            bi.nPeerIndex = Integer.valueOf(fields[2]);
            bi.nPosX = Integer.valueOf(fields[3]) + m_rtClient.left;
            bi.nPosY = Integer.valueOf(fields[4]) + m_rtClient.top;
            bi.nBoundKeyValue = Integer.valueOf(fields[5]);

            m_baseInfoList.add(bi);
        }

        invalidate();

        MyLog("DrawSS()<-");
    }

    @Override
    protected void onDraw(Canvas canvas)
    {
        MyLog("onDraw()->");

        // draw boundary
        m_paint.setColor(Color.BLUE);
        m_paint.setStyle(Paint.Style.STROKE);
        m_paint.setStrokeWidth(10);
        // canvas.drawRect(m_rtClient.left + 1, m_rtClient.top - 1, m_rtClient.width(), m_rtClient.height(), m_paint);

        m_paint.setColor(Color.BLACK);
        m_paint.setStrokeWidth(1);
        Rect rtText = new Rect();
        if (m_strMsg != null)
        {
            m_paint.setTextSize(30);

            String[] msgList = m_strMsg.split("\n");
            int nLastTop = 0;
            for (int i = 0; i < msgList.length; ++i)
            {
                m_paint.getTextBounds(msgList[i], 0, msgList[i].length() - 1, rtText);

                if (nLastTop == 0)
                    nLastTop = m_rtClient.top + m_rtClient.height() / 10;
                else
                    nLastTop += (rtText.height() * 1.2);

                canvas.drawText(msgList[i],
                    m_rtClient.left,
                    nLastTop,
                    m_paint);
            }

            MyLog("onDraw()<-");
            return;
        }

        m_paint.setTextSize(30);
        m_paint.getTextBounds("A", 0, 1, rtText);
        int nDrawTextOffsetX = (int)(rtText.width() * 0.5);
        int nDrawTextOffsetY = (int)(rtText.height() * 0.5);

        // draw base
        m_paint.setStyle(Paint.Style.FILL_AND_STROKE);
        ArrayList<Point> baseDrawPosList = new ArrayList<Point>();
        for (int i = 0; i < m_baseInfoList.size(); ++i)
        {
            BaseInfo bi = m_baseInfoList.get(i);

            int nDrawPosX = bi.nPosX;
            int nDrawPosY = bi.nPosY;

            Point ptDrawPos = new Point(nDrawPosX, nDrawPosY);
            baseDrawPosList.add(ptDrawPos);

            if (bi.nAreaType == AreaType_Stem_Acceptor)
                m_paint.setColor(Color.RED);
            else if (bi.nAreaType == AreaType_Gap_Acceptor_And_D || bi.nAreaType == AreaType_Gap_D_And_Anticodon)
                m_paint.setColor(Color.rgb(239, 224, 243));
            else if (bi.nAreaType == AreaType_Stem_D)
                m_paint.setColor(Color.rgb(245, 208, 241));
            else if (bi.nAreaType == AreaType_Loop_D)
                m_paint.setColor(Color.rgb(209, 166, 255));
            else if (bi.nAreaType == AreaType_Stem_Anticodon)
                m_paint.setColor(Color.rgb(228, 245, 177));
            else if (bi.nAreaType == AreaType_Loop_Anticodon)
                m_paint.setColor(Color.rgb(119, 210, 112));
            else if (bi.nAreaType == AreaType_Gap_Anticodon_And_Twc)
                m_paint.setColor(Color.rgb(255, 251, 151));
            else if (bi.nAreaType == AreaType_Stem_Twc)
                m_paint.setColor(Color.rgb(238, 231, 239));
            else if (bi.nAreaType == AreaType_Loop_Twc)
                m_paint.setColor(Color.rgb(191, 200, 255));

            canvas.drawCircle(nDrawPosX, nDrawPosY, rtText.width(), m_paint);

            m_paint.setColor(Color.BLACK);
            canvas.drawText(bi.strBase, nDrawPosX - nDrawTextOffsetX, nDrawPosY + nDrawTextOffsetY, m_paint);
        }

        // draw decoration/ending
        m_paint.setColor(Color.BLACK);
        int nPitch = m_baseInfoList.get(1).nPosY - m_baseInfoList.get(0).nPosY;
        canvas.drawText("5'",
                baseDrawPosList.get(0).x - nPitch,
                baseDrawPosList.get(0).y + nDrawTextOffsetY,
                m_paint);

        canvas.drawText("3'",
                baseDrawPosList.get(baseDrawPosList.size() - 1).x + nPitch - rtText.width(),
                baseDrawPosList.get(baseDrawPosList.size() - 1).y + nDrawTextOffsetY,
                m_paint);

        // draw number
        m_paint.setTextSize(22);
        for (int i = 0; i < baseDrawPosList.size(); ++i)
        {
            if (i > 0 && ((i + 1) % 5 == 0))
            {
                String strNum = Integer.toString(i + 1);
                canvas.drawText(strNum,
                        baseDrawPosList.get(i).x + nDrawTextOffsetX,
                        baseDrawPosList.get(i).y + (float)(nPitch * 0.5),
                        m_paint);
            }
        }

        // draw key line.
        for (int i = 0; i < baseDrawPosList.size(); ++i)
        {
            BaseInfo bi = m_baseInfoList.get(i);
            if (bi.nPeerIndex != -1 && bi.nBoundKeyValue != 0)
            {
                float fStartX = baseDrawPosList.get(i).x;
                float fStartY = baseDrawPosList.get(i).y;
                float fEndX = baseDrawPosList.get(bi.nPeerIndex).x;
                float fEndY = baseDrawPosList.get(bi.nPeerIndex).y;

                if (bi.nAreaType == AreaType_Stem_Acceptor || bi.nAreaType == AreaType_Stem_Anticodon)
                {
                    fStartX += nDrawTextOffsetX * 2;
                    fEndX -= nDrawTextOffsetX * 2;
                }
                else if (bi.nAreaType == AreaType_Stem_D)
                {
                    fStartY += nDrawTextOffsetY * 2;
                    fEndY -= nDrawTextOffsetY * 2;
                }
                else
                {
                    fStartY -= nDrawTextOffsetY * 2;
                    fEndY += nDrawTextOffsetY * 2;
                }

                m_paint.setStrokeWidth(bi.nBoundKeyValue * 3);
                canvas.drawLine(fStartX, fStartY, fEndX, fEndY, m_paint);
            }
        }

        // draw title.
        m_paint.setStrokeWidth(1);
        m_paint.setTextSize(50);
        String strTitle = "tRNA secondary structure: " + Integer.toString(m_baseInfoList.size()) + "bps.";
        canvas.drawText(strTitle, 20, 50, m_paint);

        MyLog("onDraw()<-");
    }
}
