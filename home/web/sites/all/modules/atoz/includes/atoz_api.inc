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
		    case "get_home":
		    	return atoz_api_get_home();
		    	break;
		    case "change_password":
		    	return atoz_api_change_password();
		    	break;
		    case "get_promotions":
		    	return atoz_api_get_promotions();
		    	break;
		    case "get_ddl_data":
		    	return atoz_api_get_ddl_data();
		    	break;
		    case "sent_noti":
		    	return atoz_api_sent_noti();
		    	break;
		    case "get_outlet_locations":
		    	return atoz_api_get_outlet_location();
		    	break;
		    case "get_notifications":
		    	return atoz_api_get_notifications();
		    	break;
		    default:
		    	return atoz_api_not_found();
		    	break;
		}

	}
?>