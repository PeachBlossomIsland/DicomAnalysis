package entity;

public class ImageInfo {
	public ImageInfo() {};
	private int id;
	private String sopInstanceId;
	private String imageDate;
	private String imageTime;
	private String hightHit;
	private String windowCenter;
	private String windowWidth;
	private String imagePath;
	private int seriesId;
	public int getId() {
		return id;
	}
	public void setId(int id) {
		this.id = id;
	}
	public String getSopInstanceId() {
		return sopInstanceId;
	}
	public void setSopInstanceId(String sopInstanceId) {
		this.sopInstanceId = sopInstanceId;
	}
	public String getImageDate() {
		return imageDate;
	}
	public void setImageDate(String imageDate) {
		this.imageDate = imageDate;
	}
	public String getImageTime() {
		return imageTime;
	}
	public void setImageTime(String imageTime) {
		this.imageTime = imageTime;
	}
	public String getHightHit() {
		return hightHit;
	}
	public void setHightHit(String hightHit) {
		this.hightHit = hightHit;
	}
	public String getWindowCenter() {
		return windowCenter;
	}
	public void setWindowCenter(String windowCenter) {
		this.windowCenter = windowCenter;
	}
	public String getWindowWidth() {
		return windowWidth;
	}
	public void setWindowWidth(String windowWidth) {
		this.windowWidth = windowWidth;
	}
	public String getImagePath() {
		return imagePath;
	}
	public void setImagePath(String imagePath) {
		this.imagePath = imagePath;
	}
	public int getSeriesId() {
		return seriesId;
	}
	public void setSeriesId(int seriesId) {
		this.seriesId = seriesId;
	}
	
}
