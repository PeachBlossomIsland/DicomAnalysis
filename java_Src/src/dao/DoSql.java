package dao;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;

import entity.ImageInfo;
import entity.PatientInfo;
import entity.SeriesInfo;
import entity.StudyInfo;
import util.DBHelp;

public class DoSql {
	public int doUpdate_Delete_Insert(String []sql) {
		Connection conn=null;
		PreparedStatement preState=null;
		int re=0;
		try {
			conn=DBHelp.getConnection();
			//预编译处理，防止sql注入
			if(sql.length==1)
			{
				preState = conn.prepareStatement(sql[0]);
			}else {
				preState = conn.prepareStatement(sql[0]);
				for(int i=1;i<sql.length;i++)
				{
					preState.setString(i, sql[i]);
				}			
			}
			re=preState.executeUpdate();
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		return re;	
	}
	public ArrayList<ImageInfo> doSelect_ImageInfo(String[] sql)
	{
		Connection conn=null;
		PreparedStatement preState=null;
		ResultSet rs=null;
		ArrayList<ImageInfo> re=new ArrayList<ImageInfo> ();
		try {
			conn=DBHelp.getConnection();
			//预编译处理，防止sql注入
			if(sql.length==1)
			{
				preState = conn.prepareStatement(sql[0]);
			}else {
				preState = conn.prepareStatement(sql[0]);
				for(int i=1;i<sql.length;i++)
				{
					preState.setString(i, sql[i]);
				}			
			
			}
			rs=preState.executeQuery();
			while(rs.next())
			{
				ImageInfo i=new ImageInfo();
				i.setId(rs.getInt("id"));
				i.setSopInstanceId(rs.getString("sopInstanceId"));
				i.setImageDate(rs.getString("imageDate"));
				i.setImageTime(rs.getString("imageTime"));
				i.setHightHit(rs.getString("hightHit"));
				i.setWindowCenter(rs.getString("windowCenter"));
				i.setWindowWidth(rs.getString("windowWidth"));
				i.setImagePath(rs.getString("imagePath"));
				i.setSeriesId(rs.getInt("seriesId"));
				re.add(i);
			}
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}finally
		{
			try {
				
				if(rs!=null)
				{
					rs.close();
					rs=null;
				}
				if(preState!=null)
				{
					preState.close();
					preState=null;
				}
			}catch(Exception e)
			{
				e.printStackTrace();
			}
		}
		return re;
	}
	
	public ArrayList<PatientInfo> doSelect_PatientInfo(String[] sql)
	{
		Connection conn=null;
		PreparedStatement preState=null;
		ResultSet rs=null;
		ArrayList<PatientInfo> re=new ArrayList<PatientInfo> ();
		try {
			conn=DBHelp.getConnection();
			//预编译处理，防止sql注入
			if(sql.length==1)
			{
				preState = conn.prepareStatement(sql[0]);
			}else {
				preState = conn.prepareStatement(sql[0]);
				for(int i=1;i<sql.length;i++)
				{
					preState.setString(i, sql[i]);
				}			
			
			}
			rs=preState.executeQuery();
			while(rs.next())
			{
				PatientInfo d=new PatientInfo();
				d.setId(rs.getInt("id"));
				d.setPatientId(rs.getString("patientId"));
				d.setPatientAddress(rs.getString("patientAddress"));
				d.setPatientAge(rs.getString("patientAge"));
				d.setPatientBirthDate(rs.getString("patientBirthDate"));
				d.setPatientBirthTime(rs.getString("patientBirthTime"));
				d.setPatientName(rs.getString("patientName"));
				d.setPatientSex(rs.getString("patientSex"));
				d.setPatientWeight(rs.getString("patientWeight"));
				re.add(d);
			}
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}finally
		{
			try {
				
				if(rs!=null)
				{
					rs.close();
					rs=null;
				}
				if(preState!=null)
				{
					preState.close();
					preState=null;
				}
			}catch(Exception e)
			{
				e.printStackTrace();
			}
		}
		return re;
	}

	
	public ArrayList<SeriesInfo> doSelect_SeriesInfo(String[] sql)
	{
		Connection conn=null;
		PreparedStatement preState=null;
		ResultSet rs=null;
		ArrayList<SeriesInfo> re=new ArrayList<SeriesInfo> ();
		try {
			conn=DBHelp.getConnection();
			//预编译处理，防止sql注入
			if(sql.length==1)
			{
				preState = conn.prepareStatement(sql[0]);
			}else {
				preState = conn.prepareStatement(sql[0]);
				for(int i=1;i<sql.length;i++)
				{
					preState.setString(i, sql[i]);
				}			
			
			}
			rs=preState.executeQuery();
			while(rs.next())
			{
				SeriesInfo d=new SeriesInfo();
				d.setId(rs.getInt("id"));
				d.setSeriesDate(rs.getString("seriesDate"));
				d.setSeriesDescription(rs.getString("seriesDescription"));
				d.setSeriesInstanceId(rs.getString("seriesInstanceId"));
				d.setSeriesNum(rs.getString("seriesNum"));
				d.setSeriesTime(rs.getString("seriesTime"));
				d.setSliceLocation(rs.getString("sliceLocation"));
				d.setSliceThickness(rs.getString("sliceThickness"));
				d.setSpacingBetweenSlices(rs.getString("spacingBetweenSlices"));
				d.setStudyModality(rs.getString("studyModality"));
				d.setStudyId(rs.getInt("studyId"));
				re.add(d);
			}
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}finally
		{
			try {
				
				if(rs!=null)
				{
					rs.close();
					rs=null;
				}
				if(preState!=null)
				{
					preState.close();
					preState=null;
				}
			}catch(Exception e)
			{
				e.printStackTrace();
			}
		}
		return re;
	}
	
	
	public ArrayList<StudyInfo> doSelect_StudyInfo(String[] sql)
	{
		Connection conn=null;
		PreparedStatement preState=null;
		ResultSet rs=null;
		ArrayList<StudyInfo> re=new ArrayList<StudyInfo> ();
		try {
			conn=DBHelp.getConnection();
			//预编译处理，防止sql注入
			if(sql.length==1)
			{
				preState = conn.prepareStatement(sql[0]);
			}else {
				//System.out.println(sql[0]);
				preState = conn.prepareStatement(sql[0]);
				for(int i=1;i<sql.length;i++)
				{
					preState.setString(i,sql[i]);
				}			
			
			}
			rs=preState.executeQuery();
			while(rs.next())
			{
				StudyInfo d=new StudyInfo();
				d.setId(rs.getInt("id"));
				d.setStudyBodyPart(rs.getString("studyBodyPart"));
				d.setStudyDate(rs.getString("studyDate"));
				d.setStudyDescription(rs.getString("studyDescription"));
				d.setStudyId(rs.getString("studyId"));
				d.setStudyInstanceId(rs.getString("studyInstanceId"));
				d.setStudyRisId(rs.getString("studyRisId"));
				d.setStudyTime(rs.getString("studyTime"));
				d.setPatientId(rs.getInt("patientId"));
				re.add(d);
			}
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}finally
		{
			try {
				
				if(rs!=null)
				{
					rs.close();
					rs=null;
				}
				if(preState!=null)
				{
					preState.close();
					preState=null;
				}
			}catch(Exception e)
			{
				e.printStackTrace();
			}
		}
		return re;
	}
	
	
	public static void main(String[] args)
	{
		DoSql d=new DoSql();
		String getStudyinfoSql[]= {"select * from studyinfo where patientId=?","252"};
		ArrayList<StudyInfo> sinfo=d.doSelect_StudyInfo(getStudyinfoSql);
		System.out.println(sinfo.get(0).getId());
		//String[] sql= {"insert into dcminfo values(?,?,?,?,?,?)",null,"12057","20100926","1.2.840.113619.2.55.3.2831193967.596.1285460208.411","1.2.840.113619.2.55.3.2831193967.596.1285460208.412.27","C:\\Users\\zhujianghui\\Pictures\\Camera Roll\\622424199610153719朱江辉.jpg"};
		//d.doUpdate_Delete_Insert(sql);
		//System.out.println(p.get(0).getPatientName());
	}
}
