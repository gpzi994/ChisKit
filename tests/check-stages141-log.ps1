$ErrorActionPreference='Stop'
$root=Join-Path $PSScriptRoot '..'
$text=Get-Content "$root/app/ui98/main.c" -Raw
$start=$text.IndexOf('static void stage141_log(void)')
$end=$text.IndexOf('static void present(void)', $start)
if($start -lt 0 -or $end -lt $start){throw 'Missing log implementation'}
$body=$text.Substring($start,$end-$start)
if($body.Contains('of_output_flush(')){throw 'Unsupported APF FLUSH reintroduced'}
foreach($required in @('cb_log141_open(','of_output_write(','fopen("slot:5","rb")','fread(check,1,part,f)','memcmp(check,log+off,part)','if(!rc)cb_stage141_log_phase="verified"')){
 if(!$body.Contains($required)){throw "Missing log guard: $required"}
}
'PASS source guards: no APF FLUSH; full file readback required before verified'
