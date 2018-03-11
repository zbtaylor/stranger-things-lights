<?php

if(isset($_POST['message'])) {
  $data = $_POST['message'];
  $file = fopen("lights.json", "w") or die("can't open file");
  fwrite($file, '{"message": "' . $data . '"}');
  fclose($file);
} else {
  die('no post data to process');
}

// Change this to the URL of your app
header( 'Location: https://stranger-things.herokuapp.com' );
