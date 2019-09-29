<?php

require_once 'config/config.php';

if (isset($_POST["register"])) {
	
	$username = $password = $email = "";
	$username_err = $password_err = $email_err = "";

    //check username 
    if (empty(trim($_POST["register-username"]))) {
        echo $username_err = "Please enter your username.";
    } else {
        $sql_username = "SELECT id FROM account WHERE username = :username";
        
        if ($stmt = $pdo->prepare($sql_username)) {
            $stmt->bindParam(":username", $param_username, PDO::PARAM_STR);
            
            $param_username = trim($_POST["register-username"]);
            
            if ($stmt->execute()) {
                if ($stmt->rowCount() > 0) {
                    echo $username_err = "This account already existing.";
                } else {
                    $username = trim($_POST["register-username"]);
                }
            } else {
                echo "Oooops... there is an internal issue. Please contact the site administrator.";
            }
        }
        unset($stmt);
	}
	
	//check password
    if (empty(trim($_POST["register-password"]))) {
        echo $password_err = "Please enter a password to continue.";
    } elseif (strlen(trim($_POST["register-password"])) < 8) {
        echo $password_err = "Your password must at least have 8 characters.";
    } else {
        $password = trim($_POST["register-password"]);
    }

    //check e-mail is using...
    if (empty(trim($_POST["register-email"]))) {
        echo $email_err = "Please enter your e-mail address.";
    } else {
        $sql_email = "SELECT id FROM account WHERE email = :email";
        
        if ($stmt = $pdo->prepare($sql_email)) {
            $stmt->bindParam(":email", $param_email, PDO::PARAM_STR);
            
            $param_email = trim($_POST["register-email"]);
            
            if ($stmt->execute()) {
                if ($stmt->rowCount() > 0) {
                    echo $username_err = "This e-mail is already in use. Please use a different e-mail.";
                } else {
                    $email = trim($_POST["register-email"]);
                }
            } else {
                echo "Oooops... there is an internal issue. Please contact the site administrator.";
            }
        }
        unset($stmt);
	}

	//there is a bug idk i have to fix it ....
	$hash = sha1(strtoupper($username) . ':' . strtoupper($password));
  
    if (empty($username_err) && empty($password_err) && empty($email_err)) {
        
        $sql = "INSERT INTO account (username, sha_pass_hash, email, expansion) VALUES (:username, :password, :email, 2)";
        
        if ($stmt = $pdo->prepare($sql)) {
            $stmt->bindParam(":username", $param_username, PDO::PARAM_STR);
            $stmt->bindParam(":password", $param_password, PDO::PARAM_STR);
			$stmt->bindParam(":email", $param_email, PDO::PARAM_STR);
            
            $param_username = $username;
            $param_password = $hash;
			$param_email = $email;
            
            if ($stmt->execute()) {
                echo "Account has been sucessfully created. Now you can login.";
            } else {
                echo "Oooops... there is an internal issue. Please contact the site administrator.";
            }
        }
        unset($stmt);
    }
    unset($pdo);
}

?>

<!doctype html>
<html lang="de">

<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">

	<link rel="stylesheet" href="static/css/bootstrap.min.css">
	<link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.1.1/css/all.css">
	<link rel="stylesheet" href="static/css/datatables.min.css">
	<link rel="stylesheet" href="static/css/fullcalendar.min.css">
	<link rel="stylesheet" href="static/css/bootadmin.min.css">

	<title>Register | OregonCore</title>
</head>

<body class="bg-light">
	<div class="container h-100">
		<div class="row h-100 justify-content-center align-items-center">
			<div class="col-md-6">
				<div class="panel panel-login">
					<h1 class="text-center mb-4">OregonCore</h1>
					<div class="panel-body">
						<div class="row">
							<div class="col-lg-12">
								<form name="register-form" id="register-form" method="POST">
								
									<div class="input-group mb-3">
										<div class="input-group-prepend">
											<span class="input-group-text"><i class="fa fa-user"></i></span>
										</div>
										<input name="register-username" type="text" class="form-control" placeholder="Accountname" required>
									</div>
									
									<div class="input-group mb-3">
										<div class="input-group-prepend">
											<span class="input-group-text"><i class="fa fa-envelope"></i></span>
										</div>
										<input name="register-email" type="email" class="form-control" placeholder="E-Mail" required>
									</div>
									
									<div class="input-group mb-3">
										<div class="input-group-prepend">
											<span class="input-group-text"><i class="fa fa-key"></i></span>
										</div>
										<input name="register-password" type="password" class="form-control" placeholder="Account Password" required>
									</div>
									
									<hr>
									<div class="form-group">
										<div class="row">
											<div class="col-sm-12 col-sm-offset-3">
												<input type="submit" name="register" id="register" class="btn btn-block btn-success" value="Create Account">
											</div>
										</div>
									</div>
									
									<hr>
								</form>
							</div>
						</div>
					</div>
				</div>
			</div>
		</div>
	</div>

	<script src="static/js/jquery.min.js"></script>
	<script src="static/js/bootstrap.bundle.min.js"></script>
	<script src="static/js/datatables.min.js"></script>
	<script src="static/js/moment.min.js"></script>
	<script src="static/js/fullcalendar.min.js"></script>
	<script src="static/js/bootadmin.min.js"></script>

</body>


</html>