<?php

	function  myatoz_member_report($parameter1=null,$parameter2=null){
		$output = '';
		if($parameter1 == 'csv'){
			$output .= myatoz_member_report_download();
		}else{
			$contentForm = drupal_get_form('myatoz_member_report_filter_form');
			$output .= drupal_render($contentForm);
			$output .= myatoz_member_report_list();
		}
		
		return $output;
	}

	function myatoz_member_report_list(){

		global $user;

		$output="";			
		$link = $_SERVER["REQUEST_URI"];
		$rowsPerPage = 100;
		$dataPerRow = 1;
		$offset = atoz_admin_page_offset($rowsPerPage);
    	$data = array();
    	$frequecny_data = array();
    	$used_data = array();

		$sortOptions = array (
			"default"			=> "gmid",
			"name"				=> "desc",
			"gmid"  			=> "desc",
			"member_level"		=> "desc",
			"collected_point"	=> "desc",						
			"current_point"		=> "desc",
			"nrc"				=> "desc",
			"phone"				=> "desc",
			"gender"			=> "desc",
			"member_type"		=> "desc",
			"member_level"		=> "desc",
			"city"				=> "desc",
			"township"			=> "desc",
			"created_by"		=> "desc",
		);

		$styleArray = array (
			"",
			"",
			"",
			"right",
			"",
			"",
		);

		$headerFields = array (			
	        t("Member ID") 				=> "gmid",
	        t("Name")      				=> "name",
	        t("Date of Birth")			=> "dob",
	        t("NRC")					=> "nrc",
	        t("Phone")					=> "phone",
	        t("Gender")					=> "gender",
	        t("Total Collected Point")	=> "collected_point",	        
	        t("Membership Type")		=> "member_type",
	        t("Member Tier")  			=> "member_level",
	        t("Stage & Region")			=> "city",
	        t("Township")				=> "township",	 
	        t("Total Redeem Point")		=> "",
	        t("Available Point")		=> "current_point",
	        t("Redemption Frequency")  	=> "",
	        t("Created By")				=> "created_by",
	        t("Created Date")			=> "created_date",	        
		);

		$collect_point_query = "select i.umid, i.gmid,i.name,i.dob,i.nrc,i.phone,i.gender,current_point,i.member_type,i.city,i.township,i.created_date,i.created_by,i.member_level,sum(p.point) collected_point from tbl_home_members_info i left join tbl_home_points p on i.gmid = p.gmid where 1=1 ";

		if(isset($_SESSION['ATOZ_MEMBER_LEVEL_FILTER'])){
			$collect_point_query .= " AND i.member_level = '". $_SESSION['ATOZ_MEMBER_LEVEL_FILTER'] ."'";
		}

		if(isset($_SESSION['ATOZ_MEMBER_ID_FILTER'])){
			$collect_point_query .= " AND i.gmid = '". $_SESSION['ATOZ_MEMBER_ID_FILTER'] ."'" ;
		}

		if(isset($_SESSION['ATOZ_MEMBER_CITY_FILTER'])){
			if($_SESSION['ATOZ_MEMBER_CITY_FILTER'] != ''){

				$collect_point_query .= " AND i.city = '". $_SESSION['ATOZ_MEMBER_CITY_FILTER'] ."'" ;

			}			
		}	
		if(isset($_SESSION['ATOZ_MEMBER_TYPE_FILTER'])){
			$collect_point_query .= " AND i.member_type = '". $_SESSION['ATOZ_MEMBER_TYPE_FILTER'] ."'" ;
		}	

		$collect_point_query .= " group by i.gmid";	

		/// check sort ////

		$sortBy = "";
		$order = "ASC";

		if (isset ($_GET["sortby"])) {
			if($_GET['sortBy']){
				$sortBy = "";
			}else{
				$sortBy = $_GET["sortby"];
			}
		}
		else if (isset ($sortOptions["default"])) {
			$sortBy = $sortOptions["default"];
		}

		if ($sortBy && isset($sortOptions[$sortBy])) {

			if (isset ($_GET["order"])) {
				$order = $_GET["order"];
			}
			else {
				$order = $sortOptions[$sortBy];
			}


			// $query->orderby ($sortBy, $order);

			$collect_point_query .= " order by ". $sortBy . " " . $order;
		}

		/// end check sort ////

		$collect_result = db_query($collect_point_query);


		$used_point_query = "select i.umid,sum(g.gpoint) used_point from tbl_home_members_info i left join tbl_home_users_redeems ur on i.gmid = ur.gmid  left join tbl_home_gift g on ur.grid = g.grid  left join tbl_home_points p on p.gmid = i.gmid group by i.gmid";

		$used_result = db_query($used_point_query);

		$redemption_frequency_query = "select m.umid,count(ur.gmid) frequnecy from tbl_home_users_redeems ur left join tbl_home_members_info m on ur.gmid = m.gmid group by ur.gmid";

		$redemption_frequency_result = db_query($redemption_frequency_query);

		if($used_result){
			foreach($used_result as $row){
				$used_data[$row->umid] = $row->used_point;
			}
		}

		if($redemption_frequency_result){
			foreach($redemption_frequency_result as $row){
				$frequency_data[$row->umid] = $row->frequnecy;
			}
		}


		$member_level_arr = atoz_member_level();

		$township_arr = atoz_townships();

		$city_arr = atoz_division();

		$created_by_arr = get_atoz_created_by_users();

		$member_type_arr = atoz_member_types();

		$gender_arr = atoz_gender();

		if($collect_result){
			foreach($collect_result as $row){

				if($row->member_level == ''){
					$member_level = '';
				}else{
					$member_level = $member_level_arr[$row->member_level];
				}

				if(isset($frequency_data[$row->umid])){
					$frequency = $frequency_data[$row->umid];
				}else{
					$frequency = 0;
				}

				if($row->member_type){
					$member_type = $member_type_arr[$row->member_type];
				}else{
					$member_type = '';
				}
				

				$list['gmid'] = $row->gmid;
				$list['name'] = $row->name;
				$list['dob'] = $row->dob;
				$list['nrc'] = $row->nrc;
				$list['phone'] = $row->phone;
				$list['gender'] = $gender_arr[$row->gender];
				$list['collected_point'] = $row->collected_point;
				$list['member_type'] = $member_type;
				$list['member_level'] = $member_level;
				$list['city']	= $city_arr[$row->city];
				$list['township'] = $township_arr[$row->township];
				$list['used_point'] = $used_data[$row->umid];
				$list['current_point'] = $row->current_point;
				$list['frequnecy'] = $frequency;
				$list['created_by']	 = $created_by_arr[$row->created_by];
				$list['created_date'] = date('Y-m-d',strtotime($row->created_date));

				$data[$row->umid] = $list;			

			}
		}

		if(isset($_SESSION['ATOZ_REDEMPTION_FREQUENCY_FILTER'])){					
			$data = array_filter($data,"frequnecy_filter");

			$total = count($data);
		}

		$query = db_select('tbl_home_members_info','m');
		$query ->fields('m',array('gmid'));

		if(isset($_SESSION['ATOZ_MEMBER_ID_FILTER'])){
			$query ->condition('member_level',$_SESSION['ATOZ_MEMBER_ID_FILTER']);
		}

		if(isset($_SESSION['ATOZ_MEMBER_LEVEL_FILTER'])){
			$query ->condition('member_level',$_SESSION['ATOZ_MEMBER_LEVEL_FILTER']);
		}	


		$header = atoz_generateHeader ($headerFields, $sortOptions);

		$pageQuery = $query;

		$pager = atoz_admin_pager_by_query($pageQuery,"m", "gmid", $dataPerRow, $rowsPerPage, $currentPage, $offset, $total);


		$output .= '<h2 class="list_header">'.t('Members').' <span class="myst-admin-title-total">('.$total.')</span></h2>';

		$output .= '<a class="downlod-csv" href="/atoz/memberreport/csv">'.t('Download CSV').'</a>';

		$output .=  $pager . atoz_admin_create_table ("home-admin-table", $header, $data, $styleArray) . $pager;

		return $output;
	}

	function myatoz_member_report_filter_form($form, &$form_state){

		if (isset ($_SESSION[ATOZ_MEMBER_ID_FILTER])) {
	        $defaultUserIDFilter = $_SESSION[ATOZ_MEMBER_ID_FILTER];
	    }
	    else {
	        $defaultUserIDFilter = "";
	    } 	

	    if (isset ($_SESSION[ATOZ_MEMBER_LEVEL_FILTER])) {
	        $defaultMemberLevelFilter = $_SESSION[ATOZ_MEMBER_LEVEL_FILTER];
	    }
	    else {
	        $defaultMemberLevelFilter = "";
	    } 	

	    if (isset ($_SESSION[ATOZ_REDEMPTION_FREQUENCY_FILTER])) {
	        $defaultRedemptionFreqFilter = $_SESSION[ATOZ_REDEMPTION_FREQUENCY_FILTER];
	    }
	    else {
	        $defaultRedemptionFreqFilter = "";
	    } 	  

	    if (isset ($_SESSION[ATOZ_MEMBER_TYPE_FILTER])) {
	        $defaultMemberTypeFilter = $_SESSION[ATOZ_MEMBER_TYPE_FILTER];
	    }
	    else {
	        $defaultMemberTypeFilter = "";
	    } 
	    
	    if (isset ($_SESSION[ATOZ_MEMBER_CITY_FILTER])) {
	        $defaultCityFilter = $_SESSION[ATOZ_MEMBER_CITY_FILTER];
	    }
	    else {
	        $defaultCityFilter = "";
	    }   

		$form = array('#id' => 'my-atoz-member-report-filter-form');
	

		$form['member_id'] = array (
		    '#title' => t('Member ID'),
	        '#type' => 'textfield',
	        '#default_value' => $defaultUserIDFilter,
	    );

	    $member_level_arr = atoz_member_level();

	    $form['member_level'] = array (
		    '#title' => t('Member Tier'),
	        '#type' => 'select',
	        '#options' => $member_level_arr,
	        '#default_value' => $defaultMemberLevelFilter,
	    );

	    $form['frequnecy'] = array (
		    '#title' => t('Frequency'),
	        '#type' => 'textfield',
	        '#default_value' => $defaultRedemptionFreqFilter,
	    );

	    $city_arr = atoz_division();

	    $form['city'] = array (
		    '#title' => t('Stage & Region'),
	        '#type' => 'select',
	        '#options' => $city_arr,
	        '#default_value' => $defaultCityFilter,
	    );

	    $member_type_arr = atoz_member_types();

	    $form['member_type'] = array (
		    '#title' => t('Member Type'),
	        '#type' => 'select',
	        '#options' => $member_type_arr,
	        '#default_value' => $defaultMemberTypeFilter,
	    );

	    $form['submit'] = array (
	        '#type' => 'submit',
	        '#weight' => 98,
	        '#value' => t('Search')
	    );

	    $form['reset'] = array (
	        '#type' => 'submit',
	        '#weight' => 99,
	        '#value' => t('Reset')
	    );

		return $form;
	}

	function myatoz_member_report_filter_form_validate($form, &$form_state){

	}

	function myatoz_member_report_filter_form_submit($form, &$form_state){

		if ($form_state["values"]["op"] == t("Reset")) {

			unset ($_SESSION[ATOZ_MEMBER_ID_FILTER]);
			unset ($_SESSION[ATOZ_MEMBER_LEVEL_FILTER]);
			unset ($_SESSION[ATOZ_REDEMPTION_FREQUENCY_FILTER]);
			unset ($_SESSION[ATOZ_MEMBER_TYPE_FILTER]);
			unset ($_SESSION[ATOZ_MEMBER_CITY_FILTER]);
			
		}else{
			
			$memberIdFilter = $form_state["values"]["member_id"];
			$memberLevelFilter = $form_state["values"]["member_level"];
			$frequencyFilter = $form_state["values"]["frequnecy"];
			$membertypeFilter = $form_state["values"]["member_type"];
			$cityFilter = $form_state["values"]["city"];
			
			if (!$memberIdFilter) {
				unset ($_SESSION[ATOZ_MEMBER_ID_FILTER]);
			}
			else {
				$_SESSION[ATOZ_MEMBER_ID_FILTER] = $memberIdFilter;
			}	

			if (!$memberLevelFilter) {
				unset ($_SESSION[ATOZ_MEMBER_LEVEL_FILTER]);
			}
			else {
				$_SESSION[ATOZ_MEMBER_LEVEL_FILTER] = $memberLevelFilter;
			}

			if (!$frequencyFilter) {
				unset ($_SESSION[ATOZ_REDEMPTION_FREQUENCY_FILTER]);
			}
			else {
				$_SESSION[ATOZ_REDEMPTION_FREQUENCY_FILTER] = $frequencyFilter;
			}

			if (!$membertypeFilter) {
				unset ($_SESSION[ATOZ_MEMBER_TYPE_FILTER]);
			}
			else {
				$_SESSION[ATOZ_MEMBER_TYPE_FILTER] = $membertypeFilter;
			}		
			
			if (!cityFilter) {
				unset ($_SESSION[ATOZ_MEMBER_CITY_FILTER]);
			}
			else {
				$_SESSION[ATOZ_MEMBER_CITY_FILTER] = $cityFilter;
			}		
			
		}

	}

	function frequnecy_filter($value)
	{		
	  	return $value['frequnecy'] == $_SESSION['ATOZ_REDEMPTION_FREQUENCY_FILTER'];
	}

	function myatoz_member_report_download(){

		$frequecny_data = array();
		
    	$used_data = array();

		set_time_limit (0);	    

		$header = array ("Member ID","Name","Date of Birth","NRC","Total Collected Point","Phone","Membership Type","Member Tier","City","Township","Redemption Point","Available Point","Redemption Frequency","Created By","Created Date");

	    header('Content-type: application/csv');
	    header ("Content-Disposition: attachment; filename=member_report.csv");
	    echo implode (",", $header) . "\n";

	    $collect_point_query = "select i.umid, i.gmid,i.name,i.dob,i.nrc,i.phone,i.gender,current_point,i.member_type,i.city,i.township,i.created_date,i.created_by,i.member_level,sum(p.point) collected_point from tbl_home_members_info i left join tbl_home_points p on i.gmid = p.gmid where 1=1 ";

		$collect_point_query .= " group by i.gmid";

		$collect_result = db_query($collect_point_query);

		$used_point_query = "select i.umid,sum(g.gpoint) used_point from tbl_home_members_info i left join tbl_home_users_redeems ur on i.gmid = ur.gmid  left join tbl_home_gift g on ur.grid = g.grid  left join tbl_home_points p on p.gmid = i.gmid group by i.gmid";

		$used_result = db_query($used_point_query);

		$redemption_frequency_query = "select m.umid,count(ur.gmid) frequnecy from tbl_home_users_redeems ur left join tbl_home_members_info m on ur.gmid = m.gmid group by ur.gmid";

		$redemption_frequency_result = db_query($redemption_frequency_query);


		if($used_result){
			foreach($used_result as $row){
				$used_data[$row->umid] = $row->used_point;
			}
		}

		if($redemption_frequency_result){
			foreach($redemption_frequency_result as $row){
				$frequecny_data[$row->umid] = $row->frequnecy;
			}
		}

		$member_level_arr = atoz_member_level();

		$city_arr = atoz_division();

		$township_arr = atoz_townships();

		$member_type_arr = atoz_member_types();

		$created_by_arr = get_atoz_created_by_users();

	    foreach ($collect_result as $row) {

	    	if($row->member_level == ''){
	    		$member_level = '';
	    	}else{
	    		$member_level = $member_level_arr[$row->member_level];
	    	}

	    	if(isset($frequecny_data[$row->umid])){
				$frequency = $frequecny_data[$row->umid];
			}else{
				$frequency = 0;
			}


			if($row->member_type == ''){
				$member_type = '';
			}else{
				$member_type = $member_type_arr[$row->member_type];				
			}

	        $member = array();
	        $member[] = $row->gmid;
			$member[] = $row->name;
			$member[] = $row->dob;
			$member[] = $row->nrc;
			$member[] = $row->phone;
			$member[] = $row->collected_point;
			$member[] = $member_type;
			$member[] = $member_level;
			$member[] = $city_arr[$row->city];
			$member[] = $township_arr[$row->township];
			$member[] = $used_data[$row->umid];
			$member[] = $row->current_point;
			$member[] = $frequency;
			$member[] = $created_by_arr[$row->created_by];
			$member[] = date('Y-m-d',strtotime($row->created_date));

	        echo implode (",", $member) . "\n";
	    }
	    exit(0);
	}
 ?>	




  