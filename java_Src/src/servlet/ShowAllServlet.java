package servlet;

import java.io.IOException;
import java.util.ArrayList;

import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import dao.DoSql;
import entity.Dcminfo;
import entity.ImageInfo;
import entity.PatientInfo;
import entity.SeriesInfo;
import entity.StudyInfo;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;

public class ShowAllServlet extends HttpServlet {

	private static final long serialVersionUID = 1L;
	public void doGet(HttpServletRequest request,HttpServletResponse response) throws IOException
	{
		System.out.println("servlet");
		request.setCharacterEncoding("utf-8");
		response.setCharacterEncoding("utf-8");
		String parameter=(String) request.getParameter("show");
		DoSql d=new DoSql();
		if(parameter.equals("all"))
		{
			//获取所有的病人姓名
			System.out.println("all");
			String p_sql[]= {"select * from patientinfo"};
			ArrayList<PatientInfo> p_info=d.doSelect_PatientInfo(p_sql);
			ArrayList<String> p = new ArrayList<String>();
			System.out.println(p_info.size());
			for(int i=0;i<p_info.size();i++)
			{
				System.out.println(p_info.get(i).getPatientName());
				p.add(p_info.get(i).getPatientName());
			}
			System.out.println(JSONArray.fromObject(p).toString());
			response.getWriter().write(JSONArray.fromObject(p).toString());
		}else {
			//获取一个完整的检查 的信息
			System.out.println(parameter);
			String patientName=parameter;
			String getPatientIdSql[]= {"select * from patientinfo where patientName=?",patientName};
			ArrayList<PatientInfo> patientInfoByName=d.doSelect_PatientInfo(getPatientIdSql);
			Integer patientSize=patientInfoByName.size();
			ArrayList<String> reinfo=new ArrayList<String>();
			for(int i=0;i<patientSize;i++)
			{
				Integer patientId=patientInfoByName.get(i).getId();
				String getStudyinfoSql[]= {"select * from studyinfo where patientId=?",patientId.toString()};
				ArrayList<StudyInfo> StudyInfoByPid=d.doSelect_StudyInfo(getStudyinfoSql);
				int studySize=StudyInfoByPid.size();
				for(int j=0;j<studySize;j++)
				{
					Integer studyId=StudyInfoByPid.get(j).getId();
					String getSeriesinfoSql[]= {"select * from seriesinfo where studyId=?",studyId.toString()};
					ArrayList<SeriesInfo> seriesInfoBySid=d.doSelect_SeriesInfo(getSeriesinfoSql);
					int seriesSize=seriesInfoBySid.size();
					for(int k=0;k<seriesSize;k++)
					{
						Integer seriesId=seriesInfoBySid.get(k).getId();
						String getImageInfoSql[]= {"select * from imageinfo where seriesId=?",seriesId.toString()};
						ArrayList<ImageInfo> imageinfoBySeid=d.doSelect_ImageInfo(getImageInfoSql);
						for(int m=0;m<imageinfoBySeid.size();m++)
						{
							Dcminfo dcminfo=new Dcminfo();
							dcminfo.setPinfo(patientInfoByName.get(i));
							dcminfo.setSinfo(StudyInfoByPid.get(j));
							dcminfo.setSeinfo(seriesInfoBySid.get(k));
							dcminfo.setIinfo(imageinfoBySeid.get(m));
							reinfo.add(JSONObject.fromObject(dcminfo).toString());
						}
					}
				}		
			}
			System.out.println("jsonArray:");
			response.getWriter().write(JSONArray.fromObject(reinfo).toString().toString());
			response.getWriter().close();
			System.out.println(JSONArray.fromObject(reinfo).toString().toString());
		}
	}
	public static void main(String[] args) {
		// TODO Auto-generated method stub

	}

}
