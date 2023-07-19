using System.Collections;
using System.Collections.Generic;
using System;
using System.IO;
using System.Text;
using UnityEngine;
using UnityEngine.UI;

public class CameraUp : MonoBehaviour
{
    private GameObject milk;
    private Transform target;//获取旋转目标
    private Bounds bounds;//获取牛奶盒内框
    private Camera camera;//绑定相机
    private Image background;//绑定UI背景
    private Light light;
    private Transform plane;

    /*采样图片相关设置*/
    private int ratio;//图片压缩比例
    private int num_pic;//图片数量
    private int rect_width;//图片宽
    private int rect_height;//图片高
    private int rect_x;//图片开始截取的x位置（从全屏左侧开始）
    private int rect_y;//图片开始截取的y位置（从全屏下侧开始）

    /*相机位置相关*/
    private float rx_min, rx_max, ry_min, ry_max, rz_min, rz_max;

    /*更换背景相关*/
    private int background_ran;
    private string background_path;//随机更换背景的路径
    private Sprite next_background;//选取更换的背景

    //public float maxField;//控制相机FieldOfView的最大角度
    //public float minField;//控制相机FieldOfView的最小角度
    /*更换相机朝向相关*/
    private float camera_forward_min;
    private float camera_forward_max;
    private float camera_forward_x;//相机朝向和物体位置偏差的3个随机数
    private float camera_forward_y;
    private float camera_forward_z;

    /*更改平面大小*/
    private float plane_scale_minx;
    private float plane_scale_maxx;
    private float plane_scale_minz;
    private float plane_scale_maxz;


    private void Start()
    {
        milk = GameObject.Find("TestCube").gameObject;
        target = GameObject.Find("TestCube").gameObject.GetComponent<Transform>();
        camera = GameObject.Find("Main Camera").gameObject.GetComponent<Camera>();
        background = GameObject.Find("BackgroundImage").gameObject.GetComponent<Image>();
        light = GameObject.Find("Directional Light").gameObject.GetComponent<Light>();
        plane = GameObject.Find("Plane").gameObject.GetComponent<Transform>();

        //图片的参数
        ratio = 1;
        num_pic = 1500;
        rect_width = 320;
        rect_height = 240;
        rect_x = (int)(Screen.width - rect_width) / 2;
        rect_y = (int)(Screen.height - rect_height) / 2;
        Debug.Log(rect_x);
        Debug.Log(rect_y);

        //改变相机距离采样坐标原点的半径
        rx_min = -0.01f;
        rx_max = 0.01f;
        ry_min = 0.3f;
        ry_max = 0.4f;
        rz_min = -0.01f;
        rz_max = 0.01f;
        camera.transform.position = new Vector3(UnityEngine.Random.Range(rx_min,rx_max), UnityEngine.Random.Range(ry_min,ry_max), UnityEngine.Random.Range(rz_min,rz_max));

        //背景随机设置
        background_ran = UnityEngine.Random.Range(1, 11);
        background_path = "Backgrounds/" + background_ran.ToString();
        Debug.Log(background_path);
        next_background = Resources.Load<Sprite>(background_path) as Sprite;
        background.sprite = next_background;


        //相机朝向随机设置
        //camera_forward_min = -0.01f;
        //camera_forward_max = 0.01f;
        //camera_forward_x = UnityEngine.Random.Range(camera_forward_min, camera_forward_max);
        //camera_forward_y = UnityEngine.Random.Range(camera_forward_min, camera_forward_max);
        //camera_forward_z = UnityEngine.Random.Range(camera_forward_min, camera_forward_max);
        //camera.transform.forward = target.position - camera.transform.position + new Vector3(camera_forward_x, camera_forward_y, camera_forward_z);//初始化相机面向物体

        //点光源位置设置
        light.transform.position = new Vector3(UnityEngine.Random.Range(-0.2f, 0.2f), 0.25f, UnityEngine.Random.Range(-0.2f, 0.2f));
        light.transform.rotation = Quaternion.Euler(UnityEngine.Random.Range(30f, 150f), UnityEngine.Random.Range(-180f, 180f), 0);
        light.intensity = UnityEngine.Random.Range(0.5f, 2.0f);

        //改变平面尺寸
        plane_scale_minx=0.01f;
        plane_scale_maxx=0.02f;
        plane_scale_minz=0.01f;
        plane_scale_maxz=0.02f;
        plane.localScale = new Vector3(UnityEngine.Random.Range(plane_scale_minx, plane_scale_maxx), 3f, UnityEngine.Random.Range(plane_scale_minz, plane_scale_maxz));

        StartCoroutine(AA());

    }

    private void Update()
    {

    }

    IEnumerator AA()
    {
        while (num_pic <= 1799)
        {
            num_pic++;
            //Debug.Log(num_pic);

            //采样，打标签
            StartCoroutine(CaptureScreenshot());

            //随机改变平面尺寸
            plane.localScale = new Vector3(UnityEngine.Random.Range(plane_scale_minx, plane_scale_maxx), 3f, UnityEngine.Random.Range(plane_scale_minz, plane_scale_maxz));

            //随机改变采样背景图
            background_ran = UnityEngine.Random.Range(1, 11);
            background_path = "Backgrounds/" + background_ran.ToString();
            Debug.Log(background_path);
            next_background = Resources.Load<Sprite>(background_path) as Sprite;
            background.sprite = next_background;

            //点光源位置设置
            light.transform.position = new Vector3(UnityEngine.Random.Range(-0.2f, 0.2f), 0.25f, UnityEngine.Random.Range(-0.2f, 0.2f));
            light.transform.rotation = Quaternion.Euler(UnityEngine.Random.Range(30f, 150f), UnityEngine.Random.Range(-180f, 180f), 0);
            light.intensity = UnityEngine.Random.Range(0.5f, 2.0f);

            //相机移动位置进行采样
            CameraMove();
            yield return new WaitForSeconds(8f);
        }
    }


    //相机围绕目标旋转
    private void CameraMove()
    {
        camera.transform.position = new Vector3(UnityEngine.Random.Range(rx_min, rx_max), UnityEngine.Random.Range(ry_min, ry_max), UnityEngine.Random.Range(rz_min, rz_max));
        //相机朝向随机
        //camera_forward_x = UnityEngine.Random.Range(camera_forward_min, camera_forward_max);
        //camera_forward_y = UnityEngine.Random.Range(camera_forward_min, camera_forward_max);
        //camera_forward_z = UnityEngine.Random.Range(camera_forward_min, camera_forward_max);
        //camera.transform.forward = target.position - camera.transform.position + new Vector3(camera_forward_x, camera_forward_y, camera_forward_z);//初始化相机面向物体
    }


    //截取固定区域
    IEnumerator CaptureScreenshot()
    {
        yield return new WaitForEndOfFrame();
        Rect rect = new Rect(rect_x, rect_y, rect_width, rect_height);//图片大小取决于ratio的大小
        Texture2D screenShot = new Texture2D(rect_width, rect_height, TextureFormat.RGB24, true);//先创建一个的空纹理，大小可根据实现需要来设置
        screenShot.ReadPixels(rect, 0, 0);//读取屏幕像素信息并存储为纹理数据，
        screenShot.Apply();
        byte[] bytes = screenShot.EncodeToPNG();//然后将这些纹理数据，成一个png图片文件

        string filename = Application.dataPath + "/milk/up4/5/" + num_pic.ToString() + ".png";
        System.IO.File.WriteAllBytes(filename, bytes);
        Debug.Log(string.Format("截屏了一张图片: {0}", filename));
    }
}
