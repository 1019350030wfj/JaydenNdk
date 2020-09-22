package com.meitu.audiovideo;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

/**
 * Author: wfj
 * Date: 2020/9/22 10:49 PM
 */
public class Utils {

    public static void writePCM(byte[] frame, String filePath) {
        try {
            File file = new File(filePath);
            FileOutputStream fos = null;
            if (!file.exists()) {
                file.createNewFile();//如果文件不存在，就创建该文件
                fos = new FileOutputStream(file);//首次写入获取
            } else {
                //如果文件已存在，那么就在文件末尾追加写入
                //这里构造方法多了一个参数true,表示在文件末尾追加写入
                fos = new FileOutputStream(file, true);
            }
            fos.write(frame);
            fos.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
