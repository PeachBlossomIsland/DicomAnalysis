<%@ page language="java"  import="java.util.*,entity.PatientInfo" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>
<!DOCTYPE html>
<html>
    <head>
    <link rel="stylesheet" href="css/showall.css" />
    <script src="https://apps.bdimg.com/libs/jquery/2.1.4/jquery.min.js"></script>
    <script type="text/javascript" src="js/showall.js"></script>
    <script src="http://libs.baidu.com/jquery/2.0.0/jquery.min.js"></script>
    <meta charset="UTF-8">
    <title>页面布局</title>
    </head>
    <body>
		 <div id="outerdiv" style="position:fixed;top:0;left:0;background:rgba(0,0,0,0.7);z-index:2;width:100%;height:100%;display:none;">
		    <div id="innerdiv" style="position:absolute;">
		        <img id="bigimg" style="border:5px solid #fff;" src="" />
		    </div>
		</div>
         <div class="container">
           <div class="header">
           	 <div class="search_box">
				<input type="text" placeholder="请输入姓名进行模糊查询">
				
	    	 </div>
           </div>
                  <div class="content_body">
                       <div class="left"></div>
	                      
                       <div class="right"></div>
                  </div>
          </div>
    </body>
</html>