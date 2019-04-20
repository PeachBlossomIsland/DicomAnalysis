package entity;

public class StudyInfo {
	public StudyInfo() {}
	private int id;
	private String studyId;
	private String studyRisId;
	private String studyInstanceId;
	private String studyDate;
	private String studyTime;
	private String studyBodyPart;
	private String studyDescription;
	private int patientId;
	public void setId(int id)
	{
		this.id=id;
	}
	public int getId()
	{
		return this.id;
	}
	public void setPatientId(int pid)
	{
		this.patientId=pid;
	}
	public int getPatientId()
	{
		return this.patientId;
	}
	public String getStudyId() {
		return studyId;
	}
	public void setStudyId(String studyId) {
		this.studyId = studyId;
	}
	public String getStudyRisId() {
		return studyRisId;
	}
	public void setStudyRisId(String studyRisId) {
		this.studyRisId = studyRisId;
	}
	public String getStudyInstanceId() {
		return studyInstanceId;
	}
	public void setStudyInstanceId(String studyInstanceId) {
		this.studyInstanceId = studyInstanceId;
	}
	public String getStudyDate() {
		return studyDate;
	}
	public void setStudyDate(String studyDate) {
		this.studyDate = studyDate;
	}
	public String getStudyTime() {
		return studyTime;
	}
	public void setStudyTime(String studyTime) {
		this.studyTime = studyTime;
	}
	public String getStudyBodyPart() {
		return studyBodyPart;
	}
	public void setStudyBodyPart(String studyBodyPart) {
		this.studyBodyPart = studyBodyPart;
	}
	public String getStudyDescription() {
		return studyDescription;
	}
	public void setStudyDescription(String studyDescription) {
		this.studyDescription = studyDescription;
	}
	
}
