package com.example.summer.afinal;

import android.app.Notification;
import android.app.NotificationManager;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.os.Bundle;
import android.os.Vibrator;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import com.amazonaws.AmazonClientException;
import com.amazonaws.AmazonServiceException;
import com.amazonaws.ClientConfiguration;
import com.amazonaws.Protocol;
import com.amazonaws.auth.BasicAWSCredentials;
import com.amazonaws.services.s3.AmazonS3Client;
import com.amazonaws.services.s3.model.PutObjectRequest;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.text.SimpleDateFormat;
import java.util.StringTokenizer;

public class MainActivity extends AppCompatActivity {

    private static final int NOTIFICATION_ID = 1;
    Socket socket;
    InetSocketAddress socketAddress;
    BufferedReader socket_in;
    PrintWriter socket_out;
    FileReceiver fr; // 사진 파일 받아오기 위한 객체
    FileReceiver fr2; // 사진 파일 안받아질 시 시연을 위해 S3 서버에 사진을 보낼 때 쓰기 위한 객체

    TextView outputL;
    TextView outputR;
    TextView car_state;
    ImageView img;

    String vive_data; //
    String d;
    String lm_str;
    String rm_str;
    int lmm_str;
    int rmm_str;
    int f_str;


    private static String bucketName = "syspro1355";  // S3 의 클라우드 파일로 업로드 될 경우 저장이 될 것임.
    private static BasicAWSCredentials Keys = new BasicAWSCredentials("AKIAJ7ORZWNK4DBV3GKA", "kLscVVE1CyirYGFmafo1bi2qPGvb7qdxRPNIzK0g"); //S3에 접근하기 위한 key값

    /* 알림 메세지 설정 */

    String Notice_msg = "";
    String Notice_0 = "사고가 일어나지 않았어. 그래도 확인해볼래?";
    String Notice_1 = "누가 스크래치 내고 갔나?? 확인해보자";
    String Notice_2 = "약간의 충격이 감지되었어. 한번 확인해보지?!";
    String Notice_3 = "누가 부딪혔나봐. 한번 확인해봐!!";
    String Notice_4 = "사고가 일어났어. 어서 확인해봐!!";
    String Notice_5 = "추돌사고!! 빨리 확인해봐!!";
    String Notice_6 = "사고가 크게 난 것 같은데?! 빨리 확인해봐!!";
    String Notice_7 = "폐차장 들어가야 하는 거 아냐?! 빨리 확인해봐!!";
    NotificationManager notificationManager; //상단바에 알림메세지 띄우기 위해서
    // 로고를 누르면 새로고침이 되서 가장 최근에 받아온 값을 출력한다.
    // 위에 소스를 그대로 복붙했다.



    // 사진 파일 안받아질 시 시연을 위해 S3 서버에 사진 파일 보내기 위해 Camera 확인 버튼에 쓰기 위한 클릭리스너
    Button.OnClickListener mClickListener = new View.OnClickListener() {
        public void onClick(View v) {

            outputL = (TextView) findViewById(R.id.outputL);
            outputR = (TextView) findViewById(R.id.outputR);
            car_state = (TextView) findViewById(R.id.textView);

            Thread worker = new Thread() {
                public void run() {
                    try {
                        Log.d("app", "sys1");
                        socketAddress = new InetSocketAddress("192.168.1.53", 8888);
                        Log.d("app", "sys2");
                        socket = new Socket();
                        socket.connect(socketAddress);
                        Log.d("app", "sys3");
                        socket_out = new PrintWriter(socket.getOutputStream(), true);
                        Log.d("app", "sys4");
                        socket_in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                        Log.d("app", "sys5");
                        fr2 = new FileReceiver(socket);

                    } catch (IOException e) {
                        Log.d("app", "sys101");
                        e.printStackTrace();
                    }
                    Log.d("app", "sys6");
                    try {
                        Log.d("app", "sys4485");
                        //data = socket_in.readLine();

                        Log.d("app", "sys7");
                        Thread worker2 = new Thread() {
                            public void run() {

                                String FileName = "/storage/emulated/0/DCIM/Camera/20150908_135011.jpg"; // 파일이름 안드로이드 폰에 있는 파일
                                ClientConfiguration s3Config = new ClientConfiguration();
                                s3Config.setMaxErrorRetry(3);
// Sets the maximum number of allowed open HTTP connections.
                                s3Config.setMaxConnections(100000);
// Sets the amount of time to wait (in milliseconds) for data
// to be transferred over a connection.
                                s3Config.setSocketTimeout(100000);
                                s3Config.setProtocol(Protocol.HTTP);
                                AmazonS3Client s3client = new AmazonS3Client(Keys, s3Config);
                                try {
                                    String time = new SimpleDateFormat("MMddhhmmss").format(new java.util.Date());
                                    Log.d("app", "Uploading a new object to S3 from a file");
                                    File file = new File(FileName);                 ///변형필요
                                    String fileName = time + ".jpg"; // syspro1355에 저장 할 파일의 이름을 설정함
                                    s3client.putObject(new PutObjectRequest(bucketName, fileName, file));


                                    Log.d("app", "업로드 완료 ~♪");/// 오류는 무시해도 됩니다.
                                } catch (AmazonServiceException ase) {
                                    Log.d("app", "Caught an AmazonServiceException, which " +
                                            "means your request made it " +
                                            "to Amazon S3, but was rejected with an error response" +
                                            " for some reason.");
                                    Log.d("app", "Error Message:    " + ase.getMessage());
                                    Log.d("app", "HTTP Status Code: " + ase.getStatusCode());
                                    Log.d("app", "AWS Error Code:   " + ase.getErrorCode());
                                    Log.d("app", "Error Type:       " + ase.getErrorType());
                                    Log.d("app", "Request ID:       " + ase.getRequestId());
                                } catch (AmazonClientException ace) {
                                    Log.d("app", "Caught an AmazonClientException, which " +
                                            "means the client encountered " +
                                            "an internal error while trying to " +
                                            "communicate with S3, " +
                                            "such as not being able to access the network.");
                                    Log.d("app", "Error Message: " + ace.getMessage());
                                }
                            }
                        };
                        worker2.start();

                        socket.close();
                    } catch (Exception e) {
                    }
                }
            };
            worker.start();
        }
    };


    // 메인 함수이다. 서버로부터 진동값을 먼저 받아오고 fileReceiver를 만들어서 파일 이름과 파일을 받아와 화면상에 진동값과 사진을 보여주고 S3서버로 사진을 현재 날짜 시간으로 이름을 변경하여 보내준다.
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        outputL = (TextView) findViewById(R.id.outputL);
        outputR = (TextView) findViewById(R.id.outputR);
        car_state = (TextView) findViewById(R.id.textView);

        Thread worker = new Thread() {
            public void run() {
                while (true) {
                    try {
                        Log.d("app", "sys11");
                        socketAddress = new InetSocketAddress("192.168.1.53", 8888);
                        socket = new Socket();//데이터 받아올 ip와 포트번호 설정
                        socket.connect(socketAddress);
                        Log.d("app", "sys12");
                        socket_out = new PrintWriter(socket.getOutputStream(), true);
                        Log.d("app", "sys13");
                        socket_in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                        Log.d("app", "sys14");
                        fr = new FileReceiver(socket);

                    } catch (IOException e) {
                        Log.d("app", "sys102");
                        e.printStackTrace();
                    }
                    Log.d("app", "sys15");
                    try {
                        //   while (true) {
                        vive_data = socket_in.readLine();
                        Log.d("app","진동값 받았다");
                        Log.d("app", "sys16");
                        fr.run();

                        Log.d("apps", String.valueOf(vive_data));
                        Log.d("app", "sys17");
                        StringTokenizer st = new StringTokenizer(vive_data, "MVT");


                        Log.d("app", "sys18");

                        while (st.hasMoreTokens()) {
                            //  Log.d("app","sys7");
                            lm_str = st.nextToken(); //왼쪽 진동값
                            rm_str = st.nextToken(); //오른쪽 진동값
                            lmm_str = Integer.parseInt(lm_str);
                            rmm_str = Integer.parseInt(rm_str);
                            f_str = (lmm_str > rmm_str) ? lmm_str : rmm_str;
                            break;
                            //Log.d("apps",String.valueOf(str[1]));
                        }

                        Vibrator vibrator = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE); // 사고가 일어날시 핸드폰에 진동을 주기위한 객체이다
                        outputL = (TextView) findViewById(R.id.outputL);
                        outputR = (TextView) findViewById(R.id.outputR);
                        car_state = (TextView) findViewById(R.id.textView);
                        outputL.setText(lm_str);
                        outputR.setText(rm_str);
                        if (f_str >= 10 && f_str < 15) {
                            Notice_msg = Notice_0;
                            car_state.setText(Notice_0);
                        } else if (f_str >= 15 && f_str < 20) {
                            Notice_msg = Notice_1;
                            car_state.setText(Notice_1);
                        } else if (f_str >= 20 && f_str < 25) {
                            Notice_msg = Notice_2;
                            car_state.setText(Notice_2);
                        } else if (f_str >= 25 && f_str < 30) {
                            Notice_msg = Notice_3;
                            car_state.setText(Notice_3);
                        } else if (f_str >= 30 && f_str < 40) {
                            Notice_msg = Notice_4;
                            car_state.setText(Notice_4);
                        } else if (f_str >= 40 && f_str < 45) {
                            Notice_msg = Notice_5;
                            car_state.setText(Notice_5);
                        } else if (f_str >= 45 && f_str < 60) {
                            Notice_msg = Notice_6;
                            car_state.setText(Notice_6);
                        } else {
                            Notice_msg = Notice_7;
                            car_state.setText(Notice_7);
                        }
                        generatNotificaion();
                        vibrator.vibrate(1000);

                        socket.close();
                        //  socket_in.close();
                        // socket_out.close();
                        //  }
                    } catch (Exception e) {
                    }
                }

            }
        };
        Log.d("app", "sys201");
        findViewById(R.id.button3).setOnClickListener(mClickListener);
        worker.start();
        Log.d("app", "sys202");
    }

    private void generatNotificaion() {
        notificationManager = (NotificationManager)
                getSystemService(Context.NOTIFICATION_SERVICE);

        Context context = MainActivity.this;
        Notification notify = new Notification.Builder(context)
                .setTicker("BlackSide 어플!")
                .setContentTitle("블랙박스 카메라 확인하세요")
                .setContentText(Notice_msg)
                .setSmallIcon(android.R.drawable.stat_notify_more)
                .setWhen(System.currentTimeMillis())
                .build();

        notificationManager.notify(NOTIFICATION_ID, notify);
    }

    @Override
    protected void onStop() {
        super.onStop();
        try {
            socket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

class FileReceiver extends Thread {
    Socket socket;
    FileOutputStream fos;
    BufferedReader socket_in;

    ImageView iv;

    private static String bucketName = "syspro1355";   /// S3 의 클라우드 버킷 "syspro1355"로 업로드 될 경우 저장이 될 것임.
    private static BasicAWSCredentials Keys = new BasicAWSCredentials("AKIAJ7ORZWNK4DBV3GKA", "kLscVVE1CyirYGFmafo1bi2qPGvb7qdxRPNIzK0g");
    // 접근 권한을 얻기 위해 접근 키와 비밀 접근 키를 입력 해당 키들은 버켓 주인의 키이다.
    public FileReceiver(Socket socket) {
        this.socket = socket;
    }

    @Override
    public void run() {
        try {
            System.out.println("파일 수신 작업을 시작합니다.");
            socket_in = new BufferedReader(new InputStreamReader(socket.getInputStream()));

            // 파일명을 전송 받고 파일명 수정.
            String fName = socket_in.readLine();
            System.out.println("파일명 " + fName + "을 전송받았습니다.");

            // 파일을 생성하고 파일에 대한 출력 스트림 생성
            File f = new File("/storage/emulated/0/DCIM/Camera/", fName);
            fos = new FileOutputStream(f);
            System.out.println(fName + "파일을 생성하였습니다.");

            BitmapFactory.Options options = new BitmapFactory.Options();
            options.inSampleSize = 8;
            Bitmap src = BitmapFactory.decodeFile("/storage/emulated/0/DCIM/Camera/pic.bmp", options);
            Bitmap resized = Bitmap.createScaledBitmap(src, 200, 200, true);
            iv.setImageBitmap(resized);


            // 바이트 데이터를 전송받으면서 기록
            int len;
            int size = 64000;
            byte[] data = new byte[size];
            while ((len = socket.getInputStream().read(data)) != -1) {
                fos.write(data, 0, len);
                fos.flush();
            }
            socket_in.close();
            fos.close();
            System.out.println("파일 수신 작업을 완료하였습니다.");
            System.out.println("받은 파일의 사이즈 : " + f.length());
            Log.d("app", "곧갑니다!");
            Thread worker2 = new Thread() {
                public void run() {  // 스레드 생성 , 생성하지 않을 경우 서버와의 충돌이 일어남.

                    String FileName = "/storage/emulated/0/DCIM/Camera/pic.bmp"; // 파일이름 안드로이드 폰에 있는 파일
                    ClientConfiguration s3Config = new ClientConfiguration();
                    s3Config.setMaxErrorRetry(3);
// Sets the maximum number of allowed open HTTP connections.
                    s3Config.setMaxConnections(100000);
// Sets the amount of time to wait (in milliseconds) for data
// to be transferred over a connection.
                    s3Config.setSocketTimeout(100000);// 소켓을 100초동안 열어둠 소켓 시간을 적게 두면 전송도중 닫혀버리는 경우가 있었음
                    s3Config.setProtocol(Protocol.HTTP); // 프로토콜 설정
                    AmazonS3Client s3client = new AmazonS3Client(Keys, s3Config);
                    try {
                        String time = new SimpleDateFormat("MMddhhmmss").format(new java.util.Date());//  버켓에 월일시간분초 의 순서로된 파일의 이름을 만들 것임.
                        File file = new File(FileName);                 /// 보낼 파일
                        String fileName = time + ".bmp"; // syspro1355에 저장 할 파일의 이름과 확장자 설정함
                        s3client.putObject(new PutObjectRequest(bucketName, fileName, file)); // syspro1355 버켓에 파일을 전송한다.


                        Log.d("app", "업로드 완료 ~♪");
                    } catch (AmazonServiceException ase) {  // 아마존 서비스와의 Exception
                        Log.d("app", "Caught an AmazonServiceException, which " +
                                "means your request made it " +
                                "to Amazon S3, but was rejected with an error response" +
                                " for some reason.");
                        Log.d("app", "Error Message:    " + ase.getMessage());
                        Log.d("app", "HTTP Status Code: " + ase.getStatusCode());
                        Log.d("app", "AWS Error Code:   " + ase.getErrorCode());
                        Log.d("app", "Error Type:       " + ase.getErrorType());
                        Log.d("app", "Request ID:       " + ase.getRequestId());
                    } catch (AmazonClientException ace) { // 서버와의 통신에서 Exception이 일어날 경우
                        Log.d("app", "Caught an AmazonClientException, which " +
                                "means the client encountered " +
                                "an internal error while trying to " +
                                "communicate with S3, " +
                                "such as not being able to access the network.");
                        Log.d("app", "Error Message: " + ace.getMessage());
                    }
                }
            };
            worker2.start();

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

}

class FileRe extends Thread {
    Socket socket;
    FileOutputStream fos;
    BufferedReader socket_in;

    ImageView iv;

    private static String bucketName = "syspro1355";  /// S3 의 클라우드 파일로 업로드 될 경우 저장이 될 것임.
    private static BasicAWSCredentials Keys = new BasicAWSCredentials("AKIAJ7ORZWNK4DBV3GKA", "kLscVVE1CyirYGFmafo1bi2qPGvb7qdxRPNIzK0g");

    public FileRe(Socket socket) {
        this.socket = socket;
    }

    @Override
    public void run() {

            Log.d("app", "곧갑니다!");
            Thread worker2 = new Thread() {
                public void run() {

                    String FileName = "/storage/emulated/0/DCIM/Camera/pic.bmp"; // 파일이름 안드로이드 폰에 있는 파일
                    ClientConfiguration s3Config = new ClientConfiguration();
                    s3Config.setMaxErrorRetry(3);
// Sets the maximum number of allowed open HTTP connections.
                    s3Config.setMaxConnections(100000);
// Sets the amount of time to wait (in milliseconds) for data
// to be transferred over a connection.
                    s3Config.setSocketTimeout(100000);
                    s3Config.setProtocol(Protocol.HTTP);
                    AmazonS3Client s3client = new AmazonS3Client(Keys, s3Config);
                    try {
                        String time = new SimpleDateFormat("MMddhhmmss").format(new java.util.Date());
                        Log.d("app", "Uploading a new object to S3 from a file");
                        File file = new File(FileName);                 ///변형필요
                        String fileName = time + ".bmp"; // syspro1355에 저장 할 파일의 이름을 설정함
                        s3client.putObject(new PutObjectRequest(bucketName, fileName, file));


                        Log.d("app", "업로드 완료 ~♪");/// 오류는 무시해도 됩니다.
                    } catch (AmazonServiceException ase) {
                        Log.d("app", "Caught an AmazonServiceException, which " +
                                "means your request made it " +
                                "to Amazon S3, but was rejected with an error response" +
                                " for some reason.");
                        Log.d("app", "Error Message:    " + ase.getMessage());
                        Log.d("app", "HTTP Status Code: " + ase.getStatusCode());
                        Log.d("app", "AWS Error Code:   " + ase.getErrorCode());
                        Log.d("app", "Error Type:       " + ase.getErrorType());
                        Log.d("app", "Request ID:       " + ase.getRequestId());
                    } catch (AmazonClientException ace) {
                        Log.d("app", "Caught an AmazonClientException, which " +
                                "means the client encountered " +
                                "an internal error while trying to " +
                                "communicate with S3, " +
                                "such as not being able to access the network.");
                        Log.d("app", "Error Message: " + ace.getMessage());
                    }
                }
            };
            worker2.start();
    }
}