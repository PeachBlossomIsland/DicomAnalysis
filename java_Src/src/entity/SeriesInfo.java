package entity;

public class SeriesInfo {
	public SeriesInfo() {}
	private int id;
	private String seriesNum;
	private String seriesInstanceId;
	private String studyModality;
	private String seriesDescription;
	private String seriesDate;
	private String seriesTime;
	private String sliceThickness;
	private String spacingBetweenSlices;
	private String sliceLocation;
	private int studyId;
	public void setId(int id)
	{
		this.id=id;
	}
	public int getId()
	{
		return this.id;
	}
	public void setStudyId(int sid)
	{
		this.studyId=sid;
	}
	public int getStudyId()
	{
		return this.studyId;
	}
	public String getSeriesNum() {
		return seriesNum;
	}
	public void setSeriesNum(String seriesNum) {
		this.seriesNum = seriesNum;
	}
	public String getSeriesInstanceId() {
		return seriesInstanceId;
	}
	public void setSeriesInstanceId(String seriesInstanceId) {
		this.seriesInstanceId = seriesInstanceId;
	}
	public String getStudyModality() {
		return studyModality;
	}
	public void setStudyModality(String studyModality) {
		this.studyModality = studyModality;
	}
	public String getSeriesDescription() {
		return seriesDescription;
	}
	public void setSeriesDescription(String seriesDescription) {
		this.seriesDescription = seriesDescription;
	}
	public String getSeriesDate() {
		return seriesDate;
	}
	public void setSeriesDate(String seriesDate) {
		this.seriesDate = seriesDate;
	}
	public String getSeriesTime() {
		return seriesTime;
	}
	public void setSeriesTime(String seriesTime) {
		this.seriesTime = seriesTime;
	}
	public String getSliceThickness() {
		return sliceThickness;
	}
	public void setSliceThickness(String sliceThickness) {
		this.sliceThickness = sliceThickness;
	}
	public String getSpacingBetweenSlices() {
		return spacingBetweenSlices;
	}
	public void setSpacingBetweenSlices(String spacingBetweenSlices) {
		this.spacingBetweenSlices = spacingBetweenSlices;
	}
	public String getSliceLocation() {
		return sliceLocation;
	}
	public void setSliceLocation(String sliceLocation) {
		this.sliceLocation = sliceLocation;
	}
	
}
