package com.dv.dvrna;

import android.app.Activity;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Point;
import android.graphics.Rect;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.app.AlertDialog;
import android.widget.Toast;

import java.util.ArrayList;

public class MainActivity extends Activity
{
    // member variables.
    public MainActivity m_mainActivity = null;
    public String m_strAppDataPath = "";

    // top/bottom bar buttons.
    private Button m_btnAdd = null;

    // ui controls.
    tRNASecondaryView m_ssView = null;

    // methods.
    private void MyLog(String strMsg)
    {
        strMsg = "[MainActivity] " + strMsg;
        Log.i("MyDebug", strMsg);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        MyLog("onCreate()->");

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // initialize UI items.
        InitComponents();

        MyLog("onCreate()<-");
    }

    private void InitComponents()
    {
        MyLog("InitComponents()->");

        m_mainActivity = this;

        // top/bottom bar.
        m_btnAdd = (Button)super.findViewById(R.id.idMainTopBarAdd);

        // ui controls.
        // m_ssView = new SSView(this);
        m_ssView = (tRNASecondaryView)super.findViewById(R.id.ssView);

        // set listener on tool bar button.
        m_btnAdd.setOnClickListener(m_toolBarBtnlistener);

        MyLog("InitComponents()<-");
    }

    private Button.OnClickListener m_toolBarBtnlistener = new Button.OnClickListener()
    {
        public void onClick(View v)
        {
            if (v == m_btnAdd)
            {
                LayoutInflater factory = LayoutInflater.from(m_mainActivity);
                final View addSSDataView = factory.inflate(R.layout.add_trna_ss_data, null);
                final EditText etSeq = (EditText)addSSDataView.findViewById(R.id.tRNASS_seq);
                final EditText etSS = (EditText)addSSDataView.findViewById(R.id.tRNASS_ss);
                final Button btSample = (Button)addSSDataView.findViewById(R.id.tRNASS_get_sample);

                AlertDialog.Builder dlgBuilder = new AlertDialog.Builder(m_mainActivity);
                dlgBuilder.setTitle("Input tRNA Secordary Data");
                dlgBuilder.setView(addSSDataView);
                dlgBuilder.setPositiveButton("OK", new DialogInterface.OnClickListener()
                    {
                        public void onClick(DialogInterface dialog, int i)
                        {
                            String strSeq = etSeq.getText().toString();
                            String strSS = etSS.getText().toString();
                            DrawSS(strSeq, strSS);
                        }
                    }
                );
                dlgBuilder.setNegativeButton("Cancel", null);

                btSample.setOnClickListener(new View.OnClickListener()
                {
                    @Override
                    public void onClick(View v)
                    {
                        String strSeq = "GGGGGTATAGCTCAGTGGTAGAGCGCGTGCTTAGCATGCACGAGGtCCTGGGTTCGATCCCCAGTACCTCCA";
                        String strSS = ">>>>>>>..>>>>.......<<<<.>>>>>.......<<<<<.....>>>>>.......<<<<<<<<<<<<.";

                        // String strSeq = "GGGCGTGTGGCGTAGTCGGTAGCGCGCTCCCTTAGCATGGGAGAGGtCTCCGGTTCGATTCCGGACTCGTCCA";
                        // String strSS = ">>>>>.>..>>>>........<<<<.>>>>>.......<<<<<.....>>>>>.......<<<<<<.<<<<<.";
                        // String strSeq = "GCAGTCATGTCCGAGTGGTtAAGGAGATTGACTAGAAATCAATTGGGCTCTGCCCGCGTAGGTTCGAATCCTGCTGACTGCG";
                        // String strSS = ">>>>>>>..>>>..........<<<.>>>>>.......<<<<<.>>>>...<<<<..>>>>>.......<<<<<<<<<<<<.";
                        etSeq.setText(strSeq);
                        etSS.setText(strSS);
                    }
                });

                final AlertDialog alertDialog = dlgBuilder.create();
                alertDialog.show();
            }
        }
    };

    private void DrawSS(String strSeq, String strSS)
    {
        MyLog("DrawSS()->");

        String strMethodName = new String("RnaGetSSDrawInfo");

        String strParam1Name = new String("Seq");
        String strParam1DataType = new String("string");
        String strParam1Value = strSeq;
        MyLog("DrawSS() strSeq = " + strSeq);

        String strParam2Name = new String("SS");
        String strParam2DataType = new String("string");
        String strParam2Value = strSS;
        MyLog("DrawSS() strSS = " + strSS);

        String strParam3Name = new String("ClientWidth");
        String strParam3DataType = new String("int");
        String strParam3Value = String.valueOf(m_ssView.GetClientWidth());

        String strParam4Name = new String("ClientHeight");
        String strParam4DataType = new String("int");
        String strParam4Value = String.valueOf(m_ssView.GetClientHeight());

        String[] inputData = new String[]
        {
            strMethodName,
            strParam1Name, strParam1DataType, strParam1Value,
            strParam2Name, strParam2DataType, strParam2Value,
            strParam3Name, strParam3DataType, strParam3Value,
            strParam4Name, strParam4DataType, strParam4Value
        };

        String[] outputData = DvRnaEntry(inputData);

        String strParam5Name = outputData[0];
        String strParam5DataType = outputData[1];
        String strParam5Value = outputData[2];
        MyLog("DrawSS() strParam5Value = " + strParam5Value);

        String strParam6Name = outputData[3];
        String strParam6DataType = outputData[4];
        String strParam6Value = outputData[5];
        MyLog("DrawSS() strParam6Value = " + strParam6Value);

        if (!strParam5Value.isEmpty())
        {
            m_ssView.ShowMsg(strParam5Value);
        }
        else
        {
            m_ssView.DrawSS(strParam6Value);
        }

        MyLog("DrawSS()<-");
    }

    /**
     * A native method that is implemented by the 'dvrnacore' native library,
     * which is packaged with this application.
     */
    public native String[] DvRnaEntry(String[] inputData);

    // Used to load the 'dvrnacore' library on application startup.
    static {
        System.loadLibrary("dvrnacore");
    }
}
