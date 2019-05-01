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
		System.out.println("servlet-Get");
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
				String pn=p_info.get(i).getPatientName();
				//去重
				if(!p.contains(pn))
				{
					System.out.println(pn);
					p.add(pn);
				}
			}
			System.out.println(JSONArray.fromObject(p).toString());
			response.getWriter().write(JSONArray.fromObject(p).toString());
		}else if(parameter.equals("del"))
		{
			String pid=(String) request.getParameter("patientid");
			System.out.println("删除的id:"+pid);
			String[] delSql= {"delete from patientinfo where id=?",pid};
			if(d.doUpdate_Delete_Insert(delSql)>0) {
				response.getWriter().write("删除成功!");
			}else {
				response.getWriter().write("删除失败!");
			}
		}
	}
	public void doPost(HttpServletRequest request,HttpServletResponse response) throws IOException
	{
		System.out.println("servlet-post");
		request.setCharacterEncoding("utf-8");
		response.setCharacterEncoding("utf-8");
		String parameter=(String) request.getParameter("show");
		DoSql d=new DoSql();
		if(parameter.equals("patientName")){
			//获取一个完整的检查 的信息
			String patientName=(String) request.getParameter("patientName");
			System.out.println(patientName);
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
		}else if(parameter.equals("save_new")){
			String new_sex=(String) request.getParameter("new_sex");
			String new_age=(String) request.getParameter("new_age");
			String new_addr=(String) request.getParameter("new_addr");
			String new_weight=(String) request.getParameter("new_weight");
			String new_studyDes=(String) request.getParameter("new_studyDes");
			String new_seriesDes=(String) request.getParameter("new_seriesDes");
			String new_studyDate=(String) request.getParameter("new_studyDate");
			String new_seriesDate=(String) request.getParameter("new_seriesDate");
			String new_imageDate=(String) request.getParameter("new_imageDate");
			String new_studyBody=(String) request.getParameter("new_studyBody");
			String new_patientId=(String) request.getParameter("new_patientId");
			String new_studyId=(String) request.getParameter("new_studyId");
			String new_seriesId=(String) request.getParameter("new_seriesId");
			System.out.println("new_patientId:"+new_patientId);
			System.out.println("new_studyId:"+new_studyId);
			System.out.println("new_seriesId:"+new_seriesId);
			String[] updatePinfo= {"update patientinfo set patientAge=?,patientAddress=?,patientWeight=?,patientSex=? where id=?",new_age,new_addr,new_weight,new_sex,new_patientId};
			String[] updateSinfo= {"update studyinfo set studyDate=?,studyBodyPart=?,studyDescription=? where id=?",new_studyDate,new_studyBody,new_studyDes,new_studyId};
			String[] updateSeinfo= {"update seriesinfo set seriesDescription=?,seriesDate=? where id=?",new_seriesDes,new_seriesDate};
			d.doUpdate_Delete_Insert(updateSeinfo);
			d.doUpdate_Delete_Insert(updateSinfo);
			d.doUpdate_Delete_Insert(updatePinfo);
			response.getWriter().print("保存修改成功!");
		}
	}
	public static void main(String[] args) {
		// TODO Auto-generated method stub

	}

}
