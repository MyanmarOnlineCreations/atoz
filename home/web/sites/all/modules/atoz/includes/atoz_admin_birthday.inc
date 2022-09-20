<?php
	function myatoz_birthday($parameter1=null,$parameter2=null){
		$output = '';

		$output .= myatoz_birthday_list();


		return $output;
	}


	function myatoz_birthday_list(){

		global $user;

		$output="";	
		$offset = 0;
		$link = $_SERVER["REQUEST_URI"];
		$rowsPerPage = 100;
		$dataPerRow = 1;
    	$data = array();

		$sortOptions = array (
			"default"			=> "created_date",
			"name"				=> "",
			"member_level"  	=> "desc",
			"phone"				=> "",
			
		);

		$styleArray = array (
			"",
			"",
			"",
		);

		$headerFields = array (
			"Name"      		=> "name",
	        "Membership Level" 	=> "member_level",
	        "Phone"				=> "phone",
	        
		);

		$query = db_select('tbl_home_members_info','m');
		$query ->fields('m',array('name','member_level','phone'));

		$query ->where("DATE_FORMAT(dob, '%d/%m') = DATE_FORMAT(CURDATE(),'%d/%m') and status = 1001");

		$result = db_query($query);

		$data = array();

		$member_level_arr = atoz_member_level();

		if($result){
			foreach($result as $row){
				$list = array();
				foreach($row as $key=>$value){	
					if($key == 'member_level'){
						$list[$key] = $member_level_arr[$value];
					}else{
						$list[$key] = $value;
					}				
				}
				$data[] = $list;
			}
		}

		$header = atoz_generateHeader ($headerFields, $sortOptions);

		$pageQuery = $query;

		$pager = atoz_admin_pager_by_query($pageQuery,"m", "gmid", $dataPerRow, $rowsPerPage, $currentPage, $offset, $total);

		$output .= '<h2 class="list_header"> Members <span class="myst-admin-title-total">('.$total.')</span></h2>';


		$output .=  $pager . atoz_admin_create_table ("home-admin-table", $header, $data, $styleArray) . $pager;

		return $output;

	}


?>