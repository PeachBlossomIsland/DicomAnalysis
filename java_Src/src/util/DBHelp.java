package util;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;

public class DBHelp {
	private static String driver="com.mysql.jdbc.Driver";
	private static String url="jdbc:mysql://localhost:3306/b2?useUnicode=true&characterEncoding=UTF-8";
	private static String username="root";
	private static String password="525369";
	private static Connection conn=null;
	//静态代码块负责加载驱动
	static
	{
		try
		{
			Class.forName(driver);
		}
		catch(Exception ex)
		{
			ex.printStackTrace();
		}
	}
	//单例模式返回
	public static Connection getConnection()
	{
		if(conn==null)
		{
			try {
				conn=DriverManager.getConnection(url, username, password);
				return conn;
			} catch (SQLException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		return conn;
		
	}
	
	
	public static void main(String[] args)
	{
		Connection conn=DBHelp.getConnection();
		if(conn!=null)
		{
			System.out.println("数据库连接正常");
		}else {
			System.out.println("数据库连接失败");
		}
	}
}
