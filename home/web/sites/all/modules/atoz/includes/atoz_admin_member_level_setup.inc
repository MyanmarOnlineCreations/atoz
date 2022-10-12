<?php
	function myatoz_member_level($parameter1=null,$parameter2=null){
		$output = '';

		$output .= myatoz_member_level_list();

		return $output;
	}

	function myatoz_member_level_list(){
		
		global $user;
		$output="";	
		$link = $_SERVER["REQUEST_URI"];
		$rowsPerPage = 100;
		$dataPerRow = 1;
		$offset = atoz_admin_page_offset($rowsPerPage);		
		
    	$data = array();

		$sortOptions = array (
			"default"			=> "created_date",
			"member_type"		=> "",
			"created_date"  	=> "desc",
			"member_tier"		=> "",
			"point_range"		=> "",
			//"no_of_staff"		=> "",
			
		);

		$styleArray = array (
			"",
			"",
			"",
			"",
			""	
		);

		$headerFields = array (
			t("Member Type")     => "member_type",
	        t("Member Tier")	  => "member_tier",
	        t("Point Range")	  => "point_range",
			//"Number of Staff"  	=> "no_of_staff",	
		);

		$query = db_select('tbl_home_member_level','m');
		$query -> fields('m');
		$result = $query->execute();

		if($result){
			$i = 0;
			foreach($result as $row){
				$list['member_type'] = $member_type_arr[$row->member_type];
				$list['member_tier'] = $row->member_tier;
				$list['point_range'] = $row->start . '-' . $row->end_point;
				//$list['no_of_staff'] = $row->name;

				$data[] = $list;				
			}

			
		}

		$header = atoz_generateHeader ($headerFields, $sortOptions);

		$pageQuery = $query;

		$output .= '<a href="/atoz/outlet/new"> +'. t('New Outlet').'</a>';


		$pager = atoz_admin_pager_by_query($pageQuery,"m", "gmlid", $dataPerRow, $rowsPerPage, $currentPage, $offset, $total);

		$output .= '<h2 class="list_header">'.t(Outlets).' <span class="myst-admin-title-total">('.$total.')</span></h2>';


		$output .=  $pager . atoz_admin_create_table ("home-admin-table", $header, $data, $styleArray) . $pager;

		return $output;


	}
?>