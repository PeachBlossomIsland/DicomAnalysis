<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
    pageEncoding="utf-8"%>
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>上传文件测试</title>
</head>
<body>
	<form method="post" action="http://localhost:8080/upload_multipart" enctype="multipart/form-data">
	    选择一个文件:
	    <input type="file" name="uploadFile" />
	    <br/><br/>
	    <input type="submit" value="上传" />
	</form>
</body>
</html>