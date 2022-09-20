<?php
	require_once ("atoz_api_controller.inc");

	function atoz_api_lists($parameter1=null,$parmeter2=null,$parameter3=null,$parameter4=null,$parameter5=null){

		// print_r($parameter1);


		switch ($parameter1) {
			case "login":	        
		        return atoz_api_login ();
		        break;
		    case "signup":
		    	return atoz_api_register();
		    	break;
		    case "get_member_info":
		    	return atoz_api_get_member_info();
		    	break;
		    case "get_point_history":
		    	return atoz_api_get_point_history();
		    	break;
		    default:
		    	return atoz_api_not_found();
		    	break;
		}

	}
?>