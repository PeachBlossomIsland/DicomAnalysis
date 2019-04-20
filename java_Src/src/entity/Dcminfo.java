package entity;
public class Dcminfo {
	private PatientInfo pinfo;
	private StudyInfo sinfo;
	private SeriesInfo seinfo;
	private ImageInfo iinfo;
	public PatientInfo getPinfo() {
		return pinfo;
	}
	public void setPinfo(PatientInfo pinfo) {
		this.pinfo = pinfo;
	}
	public StudyInfo getSinfo() {
		return sinfo;
	}
	public void setSinfo(StudyInfo sinfo) {
		this.sinfo = sinfo;
	}
	public SeriesInfo getSeinfo() {
		return seinfo;
	}
	public void setSeinfo(SeriesInfo seinfo) {
		this.seinfo = seinfo;
	}
	public ImageInfo getIinfo() {
		return iinfo;
	}
	public void setIinfo(ImageInfo iinfo) {
		this.iinfo = iinfo;
	}
	public String toString()
	{
		return null;	
	}
}
