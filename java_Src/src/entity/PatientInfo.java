package entity;

import java.util.ArrayList;

import dao.DoSql;

public class PatientInfo {
	public PatientInfo(){}
	private int id;
	private String patientName;
	private String patientAge;
	private String patientAddress;
	private String patientBirthDate;
	private String patientBirthTime;
	private String patientWeight;
	private String patientId;
	private String patientSex;
	public int getId()
	{
		return id;
	}
	public void setId(int id)
	{
		this.id=id;
	}
	public String getPatientName() {
		return patientName;
	}
	public void setPatientName(String patientName) {
		this.patientName = patientName;
	}
	public String getPatientAge() {
		return patientAge;
	}
	public void setPatientAge(String patientAge) {
		this.patientAge = patientAge;
	}
	public String getPatientAddress() {
		return patientAddress;
	}
	public void setPatientAddress(String patientAddress) {
		this.patientAddress = patientAddress;
	}
	public String getPatientBirthDate() {
		return patientBirthDate;
	}
	public void setPatientBirthDate(String patientBirthDate) {
		this.patientBirthDate = patientBirthDate;
	}
	public String getPatientBirthTime() {
		return patientBirthTime;
	}
	public void setPatientBirthTime(String patientBirthTime) {
		this.patientBirthTime = patientBirthTime;
	}
	public String getPatientWeight() {
		return patientWeight;
	}
	public void setPatientWeight(String patientWeight) {
		this.patientWeight = patientWeight;
	}
	public String getPatientId() {
		return patientId;
	}
	public void setPatientId(String patientId) {
		this.patientId = patientId;
	}
	public String getPatientSex() {
		return patientSex;
	}
	public void setPatientSex(String patientSex) {
		this.patientSex = patientSex;
	}
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		DoSql d=new DoSql();
		String p_sql[]= {"select * from patientinfo where patientName=?","Anonymized"};
		ArrayList<PatientInfo> p_info=d.doSelect_PatientInfo(p_sql);
		System.out.println(p_info.get(0).getId());
	}

}
