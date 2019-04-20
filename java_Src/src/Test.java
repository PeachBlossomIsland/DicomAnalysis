import java.util.ArrayList;

import dao.DoSql;
import entity.Dcminfo;
import entity.ImageInfo;
import entity.PatientInfo;
import entity.SeriesInfo;
import entity.StudyInfo;
import net.sf.json.JSONArray;

public class Test {
	
	public static void main(String[] args) {
		// TODO Auto-generated method stub
//		DoSql d=new DoSql();
//		String p_sql[]= {"select * from patientinfo"};
//		ArrayList<PatientInfo> p_info=d.doSelect_PatientInfo(p_sql);
//		System.out.println(p_info.size());
	
		DoSql d=new DoSql();
		String patientName="Anonymized";
		String getPatientIdSql[]= {"select * from patientinfo where patientName=?",patientName};
		ArrayList<PatientInfo> patientInfoByName=d.doSelect_PatientInfo(getPatientIdSql);
		Integer patientSize=patientInfoByName.size();
		ArrayList<String> reinfo=new ArrayList<String>();
		for(int i=0;i<patientSize;i++)
		{
			Integer patientId=patientInfoByName.get(i).getId();
			System.out.println("patientId:"+patientId);
			String getStudyinfoSql[]= {"select * from studyinfo where patientId=?",patientId.toString()};
			ArrayList<StudyInfo> StudyInfoByPid=d.doSelect_StudyInfo(getStudyinfoSql);
			int studySize=StudyInfoByPid.size();
			for(int j=0;j<studySize;j++)
			{
				Integer studyId=StudyInfoByPid.get(j).getId();
				System.out.println("studyId:"+studyId);
				String getSeriesinfoSql[]= {"select * from seriesinfo where studyId=?",studyId.toString()};
				ArrayList<SeriesInfo> seriesInfoBySid=d.doSelect_SeriesInfo(getSeriesinfoSql);
				int seriesSize=seriesInfoBySid.size();
				for(int k=0;k<seriesSize;k++)
				{
					Integer seriesId=seriesInfoBySid.get(k).getId();
					System.out.println("seriesId:"+seriesId);
					String getImageInfoSql[]= {"select * from imageinfo where seriesId=?",seriesId.toString()};
					ArrayList<ImageInfo> imageinfoBySeid=d.doSelect_ImageInfo(getImageInfoSql);
					for(int m=0;m<imageinfoBySeid.size();m++)
					{
						Dcminfo dcminfo=new Dcminfo();
						dcminfo.setPinfo(patientInfoByName.get(i));
						dcminfo.setSinfo(StudyInfoByPid.get(j));
						dcminfo.setSeinfo(seriesInfoBySid.get(k));
						dcminfo.setIinfo(imageinfoBySeid.get(m));
						//System.out.println("dcminfo:patientid:"+dcminfo.getPinfo().getId());
						System.out.println(JSONArray.fromObject(dcminfo).toString());
						reinfo.add(JSONArray.fromObject(dcminfo).toString());
					}
				}
			}		
		}
		System.out.println("");
		System.out.println("reinfo.size:"+reinfo.size());
		
		System.out.println(JSONArray.fromObject(reinfo));
		String s=JSONArray.fromObject(reinfo).toString();
		System.out.println("json:"+s);
//		for(int i=0;i<reinfo.size();i++)
//		{
//			System.out.println(reinfo.get(i).getIinfo().getImagePath());
//		}
//		System.out.println(JSONArray.fromObject(reinfo.get(0).getPinfo()).toString());
//		System.out.println(JSONArray.fromObject(reinfo).toString());
	}
}
