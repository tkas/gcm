<?php

define('MAX_LOG', 15);

$argc != 2 and die("Usage: $argv[0] gcm.db3\n\n");

$db = new SQLite3($argv[1]);

$results = $db->query('SELECT `ref_cache`, `gcid` FROM `logs` JOIN `geocaches` ON `logs`.`ref_cache` = `geocaches`.`id` GROUP BY `logs`.`ref_cache` ORDER BY `logs`.`ref_cache` ASC');

if($db->lastErrorCode() != 0)
die("Some SQLite problem!\n");

while ($row = $results->fetchArray(SQLITE3_ASSOC)) {
  echo $row['gcid'].": ";

  $logs = $db->query('SELECT `id`,`ref_cache`,`date` FROM logs WHERE `ref_cache` = "'.$row['ref_cache'].'" ORDER BY `date` DESC');
  $count=0;
  $delete_ids = '';
  while ($l = $logs->fetchArray(SQLITE3_ASSOC)) {
    $count++;
    if ($count > MAX_LOG){
      if($count % 10 == 0) echo '.';
      $delete_ids .= $l['id'].",";
    }
  }
  if ($delete_ids != ''){
    $db->exec('DELETE FROM `logs` WHERE `id` IN ('.rtrim($delete_ids, ',').') AND `ref_cache` = "'.$row['ref_cache'].'";');
  }
  $logs->finalize();
  echo $count."\n";
}

$results->finalize();
$db->close();

?>
