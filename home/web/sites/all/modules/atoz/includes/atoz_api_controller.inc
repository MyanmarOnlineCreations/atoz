<?php 

	// Status
	define ("SUCCESS_API_STATUS", "Success");
	define ("FAILED_API_STATUS", "Fail");

	//message
	define ("NEED_POST", "Request body is missing");
	define ("WRONG_USER", "User Name or Password is incorrect. Otherwise User is not active");
	define ("NO_HEADER", "Token is required");
	define ("SESSION_EXPIRE", "SESSION is expired");
	define ("APP_UPDATE", "Need App Update");
	define ("ALREADY_BIND", "Already Bind");
	define('MANDATORY_MISSING', 'Mandatory Field is required');
	define('SECURITY_KEY_INCORRECT', 'Security key is incorrect');
	define('METHOD_NOT_ALLOW', 'Method is not allowed');
	define('SUCCESS_API', 'Success');
	define('ID_NOT_FOUND','Id does not exist');
	define('URL_NOT_FOUND', 'Url does not found');
	define('FAILED_SIGN_UP','Opp! Could not Sign Up! Please contact to administrator.');
	define('SUCCESS_SIGN_UP', 'Successfully Sign Up! We will contact you as soon as possible.');
	define('WRONG_CURR_PASS', 'Current password is incorrect.Please contact to mojoenet call center.');
	define('NEW_PASS_DO_NOT_MATCH', 'New password and Confirm password are not same.');
	define('PENDING_TICKET', 'You have pending ticket.');
	define('WRONG_USER_OR_PASSWORD', 'User Id or Password is incorrect.');

	//error code
	define ("SUCCESS_API_CODE", "000");
	define ("FAILED_API_CODE", "001");
	define ("NEED_POST_CODE", "002");
	define ("WRONG_USER_CODE", "003");
	define ("NO_HEADER_CODE", "004");
	define ("SESSION_EXPIRE_CODE", "005");
	define ("APP_UPDATE_CODE", "006");
	define ("ALREADY_BIND_CODE", "007");
	define ("MANDATORY_ERR_CODE", "008");
	define('SECURITY_ERR_CODE', '009');
	define('METHOD_NOT_ALLOW_CODE', '010');
	define('ID_NOT_FOUND_CODE','011');
	define('URL_NOT_FOUND_CODE', '404');
	define('WRONG_CURR_PASS_CODE', '012');
	define('NEW_PASS_DO_NOT_MATCH_CODE', '013');
	define('PENDING_TICKET_CODE', '014');
	define('WRONG_USER_OR_PASSWORD_CODE', '015');
	define('FAILED_SIGN_UP_CODE', '016');

	define('API_KEY', '@tO2sECuriTYkEy');

	function atoz_api_login(){

		require_once DRUPAL_ROOT . '/' . variable_get('password_inc', 'includes/password.inc');

		$method = $_SERVER['REQUEST_METHOD'];

		if($method === 'POST'){

			set_http_header($method);

			$headers = apache_request_headers();

	    	$data = json_decode(file_get_contents("php://input"));

		    $response = array();

		    $result = array();

		    $now = date('Y-m-d G:i:s');

		    if(isset($headers['security_key'])){

		    	$security_key = $headers['security_key'];

		    	if(isset($data)){

			    	if($data->user_id && $data->password && $security_key && $data->app_version){

			    		if(atoz_ckeck_appversion($data->app_version)){
		    				$is_requiered_update = false;
		    			}else{
		    				$is_requiered_update = true;
		    				$updated_status = APP_UPDATE;
		    			}
		    			$isforce_update = false;

		    			if($security_key === API_KEY){

		    				$query = db_select('users','u');
		    				$query -> fields('u');
		    				$query ->condition('u.name',$data->user_id);
		    				$query ->condition('u.status',1);
		    				$user_data = $query ->execute()->fetchObject();

		    				// --- check password----
					    	if(user_check_password($data->password, $user_data)){

						        $token = atoz_generate_jwt_authentication($data->user_id,$data->password,$user_data->uid,$security_key);

						        $fields = array (			                    
				                    "token"         => $token,
				                    "updated_date"  => $now,                    
			                  	);			                  	

			                  	try{
			                  		db_merge ("tbl_home_authentication")
				                        ->key (array ("uid"=> $user_data->uid,"user_id"=> $user_data->name))
				                        ->fields ($fields)
				                        ->execute ();			                  		

			                  	}catch(Exception $e){
			                  		watchdog(
								        'authentication error',
								        'authentication error @error',
								        array('@error' => $e->getMessage()),
								        WATCHDOG_NOTICE
								    );

			                  		print_r($e->getMessage());

			                  	}

			                  	$result['uid'] = $user_data->uid;
			                  	$result['user_id'] = $user_data->name;
				              	$result['token'] = $token;

			                    $status = SUCCESS_API_STATUS;
						        $response_code = SUCCESS_API_CODE;
				              	$description = SUCCESS_API;				              	

					    	}else{
				    			$status = FAILED_API_STATUS;
					    		$response_code = WRONG_USER_CODE;
					    		$description = WRONG_USER;
					    	}

		    			}else{
				    		$status = FAILED_API_STATUS;
				    		$response_code = SECURITY_ERR_CODE;
				    		$description = SECURITY_KEY_INCORRECT;
				    	}
		    		}else{
		    			$status = FAILED_API_STATUS;
					    $response_code = MANDATORY_ERR_CODE;
					    $description = MANDATORY_MISSING;
		    		}

		    	}else{
		    	   	$status = FAILED_API_STATUS;
				    $response_code = NEED_POST_CODE;
				    $description = NEED_POST;
			    }

		    }else{
		    	$status = FAILED_API_STATUS;
		    	$response_code = NO_HEADER_CODE;
		    	$description = "Security Key is required";
		    }

		}else{
			$status = FAILED_API_STATUS;
			$response_code = METHOD_NOT_ALLOW_CODE;
			$description = METHOD_NOT_ALLOW;
		}

		$response = generate_response_arr($status,$response_code,$description,$is_requiered_update,$updated_status,$isforce_update,$result);

		$myJSON = json_encode($response);

		print_r($myJSON);

		exit();

	}


	function atoz_api_register(){

		$method = $_SERVER['REQUEST_METHOD'];

		if($method === 'POST'){

			set_http_header($method);

			$headers = apache_request_headers();

	    	$data = json_decode(file_get_contents("php://input"));

		    $response = array();

		    $result = array();

		    $now = date('Y-m-d G:i:s');

		    $name = '';
		    $dob = '';
    		$nrc = '';
    		$address = '';
    		$city = '';
    		$township = '';
    		$phone = '';
    		$password = '';
    		$confirm_password = '';

		    if(isset($data)){

				if(isset($data->name)){
		    		$name = $data->name;
		    	}

		    	if(isset($data->dob)){
		    		$dob = $data->dob;
		    	}
		    	
		    	if(isset($data->nrc)){
		    		$nrc = $data->nrc;
		    	}

		    	if(isset($data->address)){
		    		$address = $data->address;
		    	}

		    	if(isset($data->city)){
		    		$city = $data->city;
		    	}

		    	if(isset($data->township)){
		    		$township = $data->township;
		    	}

		    	if(isset($data->phone)){
		    		$phone = $data->phone;
		    	}

		    	if(isset($data->password)){
		    		$password = $data->password;

		    	}

		    	if(isset($data->confirm_password)){
		    		$confirm_password = $data->confirm_password;
		    	}

		    	if($name !='' && $dob != '' && $nrc != '' && $address != '' && $phone != '' && $password != '' && $confirm_password != ''){

		    		if($password == $confirm_password){

		    			$gmid = atoz_get_unique_profile_id('M');

			    		$account = new stdClass();

					    $account->is_new = true;
					    // temporary name
					    $username = rand(1000, 9999) . $now;    

					    $details["mail"] = $username;
					    $details["name"] = $username;
					    $details["status"] = 0; //default status is block;
					    $details["pass"] = $password;

					    $account = user_save($account,$details);
					    $uid = $account->uid;

					    if ($uid) {
					        // creating staff account
					        $username = $gmid;
					        $email = $username.'@atoz.com';
					        $details2["mail"] = $email;
					        $details2["name"] = $username;
					        $account = user_save($account,$details2);
					    }				    

			    		$fields = array(
			    			'gmid' => $gmid,
			    			'uid' => $uid,
			    			'name' => $name,
						    'dob' => $dob,
				    		'nrc' => $nrc,
				    		'address' => $address,
				    		'city' => $city,
				    		'township' => $township,
				    		'phone' => $phone,			    		
				    		'created_date' => $now,
				    		'created_by' => $uid,
			    		);

			    		$query = db_insert('tbl_home_members_info');
			    		$query ->fields($fields);
			    		$result = $query ->execute();

			    		if($result){		    			
			    			$status = SUCCESS_API_STATUS;
					    	$response_code = SUCCESS_API_CODE;
					    	$description = SUCCESS_SIGN_UP;
			    		}else{
			    			$status = FAILED_API_STATUS;
			    			$response_code = FAILED_SIGN_UP_CODE;
			    			$description = FAILED_SIGN_UP;
			    		}
		    		}else{
		    			$status = FAILED_API_STATUS;
				    	$response_code = FAILED_SIGN_UP_CODE;
				    	$description = "Password doens't match";
		    		}
		    	}else{
		    		$status = FAILED_API_STATUS;
			    	$response_code = MANDATORY_ERR_CODE;
			    	$description = MANDATORY_MISSING;
		    	}    	

			}else{
				$status = FAILED_API_STATUS;
			    $response_code = NEED_POST_CODE;
			    $description = NEED_POST;
			}

		}else{
			$status = FAILED_API_STATUS;
			$response_code = METHOD_NOT_ALLOW_CODE;
			$description = METHOD_NOT_ALLOW;
		}

		$response['status'] = $status;
		$response['response_code'] = $response_code;
		$response['description'] = $description;

		$myJSON = json_encode($response);

		print_r($myJSON);

		exit();

	}

	function atoz_api_get_member_info(){

		$method = $_SERVER['REQUEST_METHOD'];

		if($method === 'GET'){

			set_http_header($method);

			$headers = apache_request_headers();
	    	
		    $response = array();

		    $data = array();

		    $now = date('Y-m-d G:i:s');

		    if(isset($headers['token'])){

		    	$token = $headers['token'];
		    	$app_version = $_GET['app_version'];

		    	if($app_version && $token){

		    		if(atoz_ckeck_appversion($app_version)){
	    				$is_requiered_update = false;
	    			}else{
	    				$is_requiered_update = true;
	    				$updated_status = APP_UPDATE;
	    			}

	    			$isforce_update = false;

	    			$auth_data = is_auth_by_token($token);

	    			$uid = $auth_data['uid'];
	    			$user_id = $auth_data['user_id'];
	    			$isAuth = $auth_data['isAuth'];

	    			if($isAuth){

	    				$query = db_select('tbl_home_members_info','m');
	    				$query ->join('users','u','m.uid = u.uid');
	    				$query ->fields('m',array('name','dob','nrc','address','city','township','member_level','current_point'));
	    				$query ->addField('u','name','user_id');
	    				$query ->condition('u.status',1);
	    				$query ->condition('m.status','1001');
	    				$query ->condition('m.gmid',$user_id);
	    				$query ->condition('m.uid',$uid);

	    				$result = $query->execute()->fetchObject();

	    				if($result){
	    					foreach($result as $key =>$value){
	    						$data[$key] = $value;
	    					}
	    				}

	    				$status = SUCCESS_API_STATUS;
	    				$response_code = SUCCESS_API_CODE;
	    				$description = SUCCESS_API;

	    			}else{
	    				$status = FAILED_API_STATUS;
			            $response_code = SESSION_EXPIRE_CODE;
			            $description = SESSION_EXPIRE;
	    			}

		    	}else{
		    		$status = FAILED_API_STATUS;
				    $response_code = MANDATORY_ERR_CODE;
				    $description = MANDATORY_MISSING;
		    	}

		    }else{
				$status = FAILED_API_STATUS;
		    	$response_code = NO_HEADER_CODE;
		    	$description = NO_HEADER;
			}


		}else{
			$status = FAILED_API_STATUS;
			$response_code = METHOD_NOT_ALLOW_CODE;
			$description = METHOD_NOT_ALLOW;
		}


		$response = generate_response_arr($status,$response_code,$description,$is_requiered_update,$updated_status,$isforce_update,$data);

		$myJSON = json_encode($response);

		print_r($myJSON);

		exit();
	
	}

	function atoz_api_get_point_history(){

		$method = $_SERVER['REQUEST_METHOD'];

		if($method === 'GET'){

			set_http_header($method);

			$headers = apache_request_headers();
	    	
		    $response = array();

		    $data = array();

		    $now = date('Y-m-d G:i:s');

		    if(isset($headers['token'])){

		    	$token = $headers['token'];
		    	$app_version = $_GET['app_version'];

		    	if($app_version && $token){

		    		if(atoz_ckeck_appversion($app_version)){
	    				$is_requiered_update = false;
	    			}else{
	    				$is_requiered_update = true;
	    				$updated_status = APP_UPDATE;
	    			}

	    			$isforce_update = false;

	    			$auth_data = is_auth_by_token($token);

	    			$uid = $auth_data['uid'];
	    			$user_id = $auth_data['user_id'];
	    			$isAuth = $auth_data['isAuth'];

	    			if($isAuth){

	    				$query = db_select('tbl_home_points','p');	 
	    				$query ->join('tbl_home_members_info','m','p.gmid = m.gmid');   	
	    				$query ->join('tbl_home_users_info','i','p.created_by = i.uid');
	    				$query ->join('tbl_home_outlets','o','i.outlet = o.goid');			
	    				$query ->fields('m',array('current_point'));
	    				$query ->fields('p',array('point','created_date'));
	    				$query ->fields('o',array('name'));
	    				$query ->condition('m.status','1001');
	    				$query ->condition('m.gmid',$user_id);
	    				$query ->condition('m.uid',$uid);
	    				$query ->orderBy('created_date','desc');


	    				$result = $query->execute();

	    				// print_r($result);exit();

	    				if($result){
	    					foreach($result as $row){

	    						$list = array();

	    						$desc = $row->point . ' points Collected at ' . $row->name . ' - ' . date('d-M-y',strtotime($row->created_date));

	    						$list['desc'] = $desc;
	    						$list['created_date'] = $row->created_date;

	    						$arr_key = date('M',strtotime($row->created_date));

	    						// print_r($arr_key);exit();

	    						$data[$arr_key][] = $list;

	    						usort($data[$arr_key], "json_array_sort");
	    						
	    					}
	    				}


	    				$query = db_select('tbl_home_users_redeems','r');
	    				$query ->join('tbl_home_members_info','m','r.gmid = m.gmid');
	    				$query ->join('tbl_home_gift','g','r.grid = g.grid');
	    				$query ->join('tbl_home_users_info','i','r.created_by = i.uid');
	    				$query ->join('tbl_home_outlets','o','i.outlet = o.goid');
	    				$query ->fields('m',array('current_point'));
	    				$query ->addField('g','gpoint','point');
	    				$query ->fields('r',array('created_date'));	    				
	    				$query ->fields('o',array('name'));
	    				$query ->condition('r.gmid',$user_id);
	    				$query ->condition('m.status','1001');
	    				$query ->orderBy('created_date','desc');

	    				$result = $query ->execute();

	    				if($result){
	    					foreach($result as $row){
	    						$list = array();
	    						$list = array();

	    						$desc = '-' .$row->point . ' points Redeemed at ' . $row->name . ' - ' . date('d-M-y',strtotime($row->created_date));

	    						$list['desc'] = $desc;
	    						$list['created_date'] = $row->created_date;

	    						$arr_key = date('M',strtotime($row->created_date));

	    						$data[$arr_key][] = $list;

	    						usort($data[$arr_key], "json_array_sort");

	    					}
	    				}


	    				$status = SUCCESS_API_STATUS;
	    				$response_code = SUCCESS_API_CODE;
	    				$description = SUCCESS_API;

	    			}else{
	    				$status = FAILED_API_STATUS;
			            $response_code = SESSION_EXPIRE_CODE;
			            $description = SESSION_EXPIRE;
	    			}

		    	}else{
		    		$status = FAILED_API_STATUS;
				    $response_code = MANDATORY_ERR_CODE;
				    $description = MANDATORY_MISSING;
		    	}

		    }else{
				$status = FAILED_API_STATUS;
		    	$response_code = NO_HEADER_CODE;
		    	$description = NO_HEADER;
			}


		}else{
			$status = FAILED_API_STATUS;
			$response_code = METHOD_NOT_ALLOW_CODE;
			$description = METHOD_NOT_ALLOW;
		}

		$response['status'] = $status;
		$response['response_code'] = $response_code;
		$response['description'] = $description;
		$response['is_requiered_update'] = $is_requiered_update;
		$response['updated_status'] = $updated_status;
		$response['isforce_update'] = $isforce_update;
		$response['details'] = $data;

		$myJSON = json_encode($response);

		print_r($myJSON);

		exit();
	}

	function atoz_api_testing(){
		$response = array();
		$response['status'] = $status;
		$response['response_code'] = '000';

		$myJSON = json_encode($response);
		print_r($myJSON);
		exit();
	}

	function atoz_api_not_found(){

		$status = FAILED_API_STATUS;
		$response_code = URL_NOT_FOUND_CODE;
		$description = URL_NOT_FOUND;

		$response = array(
			"status"=>$status,
			"response_code"=>$response_code,
			"description"=>$description,
		);
		$myJSON = json_encode($response);
		print_r($myJSON);
		exit();
	}

	function atoz_ckeck_appversion($app_version){
    
	    $android_app_version = $app_version;
	    $setting_query = db_select("tbl_home_setting", "s");
	    $setting_query->fields ("s");
	    $setting_query->condition("s.name", "app_version");
	    $setting_result = $setting_query->execute()->fetchObject();
	    $atoz_app_version = $setting_result->value;
	    if($atoz_app_version == $android_app_version){
	      return true;
	    }else{
	       return false;
	    }
	}

	function atoz_generate_jwt_authentication($user_id,$password,$customer_id,$security_key){
	   
	    require_once ("jwt_auth.inc");

	    if(function_exists('mcrypt_create_iv')){
	        $tokenId    = base64_encode(mcrypt_create_iv(32,MCRYPT_DEV_URANDOM));
	    }else if(function_exists('openssl_random_pseudo_bytes')){        
	        $tokenId = base64_encode(openssl_random_pseudo_bytes(32));        
	    }else{
	        $tokenId = rand();
	    }

	    $issuedAt   = time();
	    $notBefore  = $issuedAt + 10;             //Adding 10 seconds
	    $expire     = $notBefore + 60;            // Adding 60 seconds
	    $serverName = ATOZ_SERVER_NAME; // Retrieve the server name from config file       

	    /*
	     * Create the token as an array
	     */
	    $data = [
	        'iat'  => $issuedAt,         // Issued at: time when the token was generated
	        'jti'  => $tokenId,          // Json Token Id: an unique identifier for the token
	        'iss'  => $serverName,       // Issuer
	        'nbf'  => $notBefore,        // Not before
	        'exp'  => $expire,           // Expire
	        'data' => [                  // Data related to the signer user
	            'user_id'   => $user_id, // userid from the users table
	            'password'  => $password,
	            'customer_id'=> $customer_id,
	        ]
	    ];


	    $secretKey = base64_decode($security_key);

	    $jwt = jwt_encode(
	        $data,      //Data to be encoded in the JWT
	        $secretKey, // The signing key
	        'HS512'     // Algorithm used to sign the token, see https://tools.ietf.org/html/draft-ietf-jose-json-web-algorithms-40#section-3
	    );
	    return $jwt;

	}
	
	function set_http_header($method){
		header("Access-Control-Allow-Origin: *");
	    header("Content-Type: application/json; charset=UTF-8");
	    header("Access-Control-Allow-Methods: ". $method);
	    header("Access-Control-Max-Age: 3600");
	    header("Access-Control-Allow-Headers: Content-Type, Access-Control-Allow-Headers, X-Requested-With");
	}


	function generate_response_arr($status,$response_code,$description,$is_requiered_update,$updated_status,$isforce_update,$data = array()){

		$response = array();
				
		$response['status'] = $status;
	    $response['response_code'] = $response_code;
	    $response['description'] = $description;
	    if($response_code == SUCCESS_API_CODE){
	    	$response['is_requiered_update'] = $is_requiered_update;
	    	$response['isforce_update'] = $isforce_update;
	    	if($is_requiered_update){
		    	$response['updated_status'] = $updated_status;
		    }
		    $response['details'] = $data; 
	    }       
	    
	    return $response;
	}

	function generate_response_arr_without_details($status,$response_code,$description,$is_requiered_update=false,$updated_status='',$isforce_update=false){
		$response = array();
		$response['status'] = $status;
		$response['response_code'] = $response_code;
		$response['description'] = $description;
		$response['is_requiered_update'] = $is_requiered_update;
		if($updated_status != ''){
			$response['updated_status'] = $updated_status;
		}
		$response['isforce_update'] = $isforce_update;
		
		return $response;
	}

	function is_auth_by_token($token){

		$response = array();

		$uid = 0;
		$user_id = '';
		$isAuth = false;

		$query = db_select('tbl_home_authentication','a');
		$query ->condition('a.token',$token);
		$query ->fields('a',array('uid','user_id'));
		$result = $query ->execute()->fetchObject();

		if($result){
			$uid = $result->uid;
			$user_id = $result->user_id;
		}

	    if($uid > 0){
	    	$isAuth = true;
	    }

	    $response['isAuth'] = $isAuth;
    	$response['uid'] = $uid;
    	$response['user_id'] = $user_id;

        return $response;
	}


		function json_array_sort($a, $b)
	    {
	        if ($a['created_date'] == $b['created_date']) return 0;
	        return ($a['created_date'] < $b['created_date']) ? 1 : -1;

	    }


?>