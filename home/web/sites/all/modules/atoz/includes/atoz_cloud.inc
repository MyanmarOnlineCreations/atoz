<?php
require_once ("atoz_cloud_config.inc");

use Aws\S3\S3Client;
use Aws\Common\Credentials\Credentials;

class MyatozCloud {
	// using the access key of the S3Upload user
	private $aws_key = MY_CLOUD_AWS_KEY;
	private $aws_secret = MY_CLOUD_AWS_SECRET;
	// instance of s3 client
	private $s3Client = null;
	// to control debug logging
	private $debug_echo = MY_CLOUD_DEBUG_ECHO;
	private $debug_error_log = MY_CLOUD_DEBUG_ERROR;
	// default bucket if none specified by caller
	private $default_bucket = MY_CLOUD_DEFAULT_BUCKET;
	// region
	private $region_endpoint = MY_CLOUD_REGION_ENDPOINT;
	private $region = MY_CLOUD_REGION;

  public function __construct () {
		// Include the SDK using the Composer autoloader

    if (!defined("DRUPAL_ROOT")) {
      error_log("MyatozCloud ERROR: DRUPAL_ROOT not defined");
    }

		$filepath = DRUPAL_ROOT . "/sites/all/libraries/aws_sdk";

		require_once ($filepath . "/aws-autoloader.php");

		// Instantiate the S3 client
		$credentials = new Credentials($this->aws_key, $this->aws_secret);
		$this->s3Client = S3Client::factory(array(
			'credentials' => $credentials,
      'region'      => $this->region,
		));
  }

  public function file_url ($filepath) {
    return $this->file_url_for_bucket($filepath, $this->default_bucket);
  }

  public function file_url_for_bucket ($filepath, $bucket) {
    return 'http://' . $bucket . $filepath;
  }

  public function does_file_exists ($key) {
    return $this->does_file_exists_in_bucket($key, $this->default_bucket);
  }

	public function does_file_exists_in_bucket ($key, $bucket) {
		try {
			$result = $this->s3Client->doesObjectExist($bucket, $key);
		} catch (Exception $e) {
			$result = false;
			$this->logexception("exist file", $e);
		}

		return $result;
	}

	public function remove_file ($key) {
		return $this->remove_file_from_bucket($key, $this->default_bucket);
	}

	public function remove_file_from_bucket ($key, $bucket) {
		// Upload an object by streaming the contents of a file
		// $filePath should be absolute path to a file on disk

		try {
			$result = $this->s3Client->deleteObject(array(
				'Bucket'     => $bucket,
				'Key'        => $key,
			));
			$result = $this->result_from_success($result);
		} catch (Exception $e) {
			$result = $this->result_from_exception($e);
			$this->logexception("remove file", $e);
		}

		return $result;
	}

	public function upload_file ($filePath, $key, $acl='public-read', $wait=true) {
		return $this->upload_file_to_bucket($filePath, $key, $this->default_bucket, $acl, $wait);
	}

	public function upload_file_to_bucket ($filePath, $key, $bucket, $acl='public-read', $wait=true) {
		// Upload an object by streaming the contents of a file
		// $filePath should be absolute path to a file on disk
		try {
			$result = $this->s3Client->putObject(array(
				'Bucket'     => $bucket,
				'Key'        => $key,
				'SourceFile' => $filePath,
				'ACL'		 => $acl,
			));
			if ($wait) {
				$this->wait_exists($bucket, $key);
			}
			$result = $this->result_from_success($result);
		} catch (Exception $e) {
			$result = $this->result_from_exception($e);
			$this->logexception("upload_file", $e);
		}

		return $result;
	}

	public function upload_data ($body, $key, $acl='public-read', $wait=true) {
		return $this->upload_data_to_bucket($body, $key, $this->default_bucket, $acl, $wait);
	}

	public function upload_data_to_bucket ($body, $key, $bucket, $acl='public-read', $wait=true) {
		// Create file in bucket, any existing file is overwritten
		try {
			$result = $this->s3Client->putObject(array(
				'Bucket' => $bucket,
				'Key'    => $key,
				'Body'   => $body,
				'ACL'    => $acl,
			));
			if ($wait) {
				$this->wait_exists($bucket, $key);
			}
			$result = $this->result_from_success($result);
		} catch (Exception $e) {
			$result = $this->result_from_exception($e);
			$this->logexception("upload_data", $e);
		}

		return $result;
	}

	public function wait_exists ($bucket, $key) {
		// We can poll the object until it is accessible
		try {
			$this->s3Client->waitUntil('ObjectExists', array(
				'Bucket' => $bucket,
				'Key'    => $key,
			));
		} catch (Exception $e) {
			$this->logexception("wait_exists", $e);
		}
	}

	public function call_failed($result) {
		return isset($result['MSCloudErr']) ? $result['MSCloudErr'] : 0;
	}

	public function call_succeeded($result) {
		return !$this->call_failed($result);
	}

	public function full_file_path($fileName, $uri='public://') {
		if ($wrapper = file_stream_wrapper_get_instance_by_uri($uri)) {
			$path = $wrapper->realpath() . $fileName;
		} else {
			$path = '';
			$this->logerror("full_file_path: no path for uri:$uri");
		}
		return $path;
	}

	public function debug_test () {
		$this->debug_echo = true; // start echoing logging to browser

		$this->log("debug_test enter");

		$bucket = $this->default_bucket;

		// list objects in the bucket
		$this->debug_list_objects($bucket);

		// upload file to bucket
		$publicID = '10207223963385930';
		$fileName = '/pictures/' . $publicID . '.jpg';
		$filePath = $this->full_file_path($fileName); // this is the full path to the file
		if ($filePath) {
			$this->log("filePath:$filePath");

			$key = $fileName; // key is the file name in the cloud, including sub-directories within the bucket
			$result = $this->upload_file_to_bucket($filePath, $key, $bucket);
			//$result = $this->upload_file($filePath, $key);

			if ($this->call_succeeded($result)) {
				// show file s3 details
				$this->log("SUCCEEDED result:".print_r($result,true));
			} else {
				$this->log("FAILED: result:".print_r($result,true));
			}

		} else {
			$this->logerror("publicPath not defined");
		}

		$this->log("debug_test exit");

		$this->debug_echo = false; // stop echoing logging to browser
	}

	public function debug_list_objects ($bucket) {
		try {
			$iterator = $this->s3Client->getIterator('ListObjects', array(
				'Bucket' => $bucket,
			));

			$this->log('Files in bucket:'.$bucket);
			foreach ($iterator as $object) {
				$this->log($object['Key']);
			}
			$result = $this->result_from_success();
		} catch (Exception $e) {
			$this->logexception("debug_list_objects", $e);
			$result = $this->result_from_exception($e);
		}
		return $result;
	}

	public function file_exists ($key) {
		// We can poll the object until it is accessible

		$exists = false;

		try {
			$exists = $this->s3Client->doesObjectExist($this->default_bucket, $key);
		} catch (Exception $e) {
			$this->logexception("file_exists", $e);
		}

		return $exists;
	}

	private function result_from_success($result=null) {
		$arr = array('MSCloudErr' => 0, 'MSCloudMsg' => 'Success');
		if ($result) {
			if (isset($result['ObjectURL'])) {
				$arr['MSCloudUrl'] = $result['ObjectURL'];
				$arr['MSCloudUrl'] = str_replace($this->region_endpoint.'/', '', $arr['MSCloudUrl']);
				$arr['MSCloudUrl'] = str_replace('https:', 'http:', $arr['MSCloudUrl']);
			}
			//$arr['MSCloudResult'] = $result;
		}
		return $arr;
	}

	private function result_from_exception($e) {
		$arr = array('MSCloudErr' => $e->getCode() ? $e->getCode() : 1, 'MSCloudMsg' => $e->getMessage());
		return $arr;
	}

	public function log ($msg) {
		if ($this->debug_echo) echo('['. get_class($this) . '] ' . $msg . '<br>');
		if ($this->debug_error_log) error_log('['. get_class($this) . '] ' . $msg . '<br>');
	}

	public function logerror ($msg) {
		if ($this->debug_echo) echo('['. get_class($this) . '] ERROR: ' . $msg . '<br>');
		if ($this->debug_error_log) error_log('['. get_class($this) . '] ERROR: ' . $msg . '<br>');
	}

	public function logexception ($msg, $e) {
		if ($this->debug_echo) echo('['. get_class($this) . '] EXCEPTION: ' . $msg . ' {' . $e->getMessage() . '}' . '<br>');
		if ($this->debug_error_log) error_log('['. get_class($this) . '] EXCEPTION: ' . $msg . ' {' . $e->getMessage() . '}' . '<br>');
	}
}
