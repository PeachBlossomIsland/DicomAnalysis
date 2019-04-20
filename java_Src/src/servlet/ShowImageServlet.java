package servlet;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

public class ShowImageServlet extends HttpServlet {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	@Override
	public void doGet(HttpServletRequest request,HttpServletResponse response) throws IOException
	{
		request.setCharacterEncoding("utf-8");
		response.setCharacterEncoding("utf-8");
		String imgPath=request.getParameter("path");
		System.out.println("imgPath:"+imgPath);
		//想要返回图片的路径
		FileInputStream fis = new FileInputStream(imgPath) ;
		//得到文件大小
		int size = fis.available();
		byte data[] = new byte[size] ;
		fis.read(data) ;
		String content = new sun.misc.BASE64Encoder().encode(data); // 具体的编码方法  
		fis.close();
				
		//设置返回的文件类型
		response.setContentType("image/png");
	//	OutputStream os = response.getOutputStream() ;
		response.getWriter().write(content);
		
		
		
		 File file = new File("C:\\Users\\zhujianghui\\Downloads\\dcm文件\\a.txt");
	        OutputStream outputStream = null;
	        if (!file.exists()) {
	            try {
	                // 如果文件找不到，就new一个
	                file.createNewFile();
	            } catch (IOException e) {
	                e.printStackTrace();
	            }
	        }
	        try {
	            // 定义输出流，写入文件的流
	            outputStream = new FileOutputStream(file);
	        } catch (FileNotFoundException e) {
	            e.printStackTrace();
	        }
	        // 定义将要写入文件的数据
	        String s="-------------------------";
	        // 把string转换成byte型的，并存放在数组中
	        byte[] bs = content.getBytes();
	        byte[] bc=s.getBytes();
	        try {
	            // 写入bs中的数据到file中
	            outputStream.write(bs);
	            outputStream.write(bc);
	        } catch (IOException e) {
	            e.printStackTrace();
	        }
	        
	        try {
	            outputStream.close();
	        } catch (IOException e) {
	            e.printStackTrace();
	        }
	        
	
		
		
		
		
		
		
		
		System.out.println(content);
		System.out.println();
		response.getWriter().close();
//		os.write(data);
//		System.out.println("data:");
//		System.out.println(data);
	   //os.flush();
		//os.close();
		
		
		
	}

}
