$xrSpecPath   = "$PSScriptRoot\..\build\_deps\openxr-src\specification\registry\xr.xml"
$templatePath = "$PSScriptRoot\..\src\openxrexplorer\openxr_properties.cpp"
$startMarker  = "// <<GENERATED_CODE_START>>"
$endMarker    = "// <<GENERATED_CODE_END>>"

# Load XML and generate template content
[xml]$registry = Get-Content $xrSpecPath

# Step 1: Find all struct types that extend XrSystemProperties
$systemPropertiesStructs = $registry.SelectNodes("//types/type[@category='struct' and @structextends='XrSystemProperties']")

# Step 2: For each struct, find which extension references it
$structToExtensionMap = @{}
foreach ($struct in $systemPropertiesStructs) {
	$structName = $struct.name

	# Search for this type name in extension requirements
	$typeRef = $registry.SelectSingleNode("//extensions/extension/require/type[@name='$structName']")

	if ($typeRef) {
		$extension = $typeRef.ParentNode.ParentNode
		$structToExtensionMap[$structName] = $extension
	} else {
		$structToExtensionMap[$structName] = $null
	}
}

# Sort structs by name before generating content
$systemPropertiesStructs = $systemPropertiesStructs | Sort-Object name

# Step 3: Generate the content
$generatedContent = @()
$idx = 0;
foreach ($struct in $systemPropertiesStructs) {
	$structName = $struct.name
	$extension  = $structToExtensionMap[$structName]
	$members    = $struct.SelectNodes("member")

	$typeMember = $members | Where-Object { 
		$_.SelectSingleNode("name").InnerText -eq "type" 
	}
	$xrStructureType = if ($typeMember -and $typeMember.values) { 
		$typeMember.values 
	} else { 
		"#pragma error NO_TYPE_FOUND"
	}

	if ($extension) {
		$generatedContent += "	// $($extension.name)"
	}
	$generatedContent += "	$structName props$idx = { $xrStructureType };"
	$generatedContent += "	sys_props.next = &props$idx;"
	$generatedContent += "	error = xrGetSystemProperties(xr_instance, xr_system_id, &sys_props);"
	$generatedContent += "	if (XR_FAILED(error)) properties_err = openxr_result_string(error);"
	$generatedContent += "	table = {};"
	$generatedContent += "	table.error        = properties_err;"
	$generatedContent += "	table.tag          = display_tag_properties;"
	$generatedContent += "	table.show_type    = true;"
	$generatedContent += "	table.column_count = 2;"
	$generatedContent += "	table.name_func    = `"xrGetSystemProperties`";"
	$generatedContent += "	table.name_type    = `"$structName`";"
	$generatedContent += "	table.spec         = `"$structName`";"

	foreach ($member in $members) {
		$memberName = $member.SelectSingleNode("name").InnerText
		$memberType = $member.SelectSingleNode("type").InnerText
		
		if ($memberName -ne "type" -and $memberName -ne "next") {
			$generatedContent += "	table.cols[0].add({`"$memberName`"});"

			if ($memberType -eq "XrBool32") { 
				$generatedContent += "	table.cols[1].add({props$idx.$memberName ? `"True`":`"False`"});" 
			}
			elseif ($memberType -eq "uint32_t") { 
				$generatedContent += "	table.cols[1].add({new_string(`"%u`", props$idx.$memberName)});" 
			}
			else { 
				#$generatedContent += "#pragma error Unimplemented struct type" 
				$generatedContent += "	table.cols[1].add({`"N/I`"});" 
			}
		}
	}
	$generatedContent += "	xr_tables.add(table);"
	$generatedContent += ""

	$idx += 1
}

# Replace the placeholder section with generated content

# Read the template file
if (-not (Test-Path $templatePath)) {
	Write-Host "ERROR: Template file not found at: $templatePath" -ForegroundColor Red
	Write-Host "Please ensure the template file exists with the required placeholder markers:" -ForegroundColor Red
	Write-Host "  $startMarker" -ForegroundColor Yellow
	Write-Host "  $endMarker" -ForegroundColor Yellow
	exit 1
}

$templateContent = Get-Content $templatePath -Raw
if ($templateContent -match "(?s)$([regex]::Escape($startMarker)).*?$([regex]::Escape($endMarker))") {
	$replacement  = $startMarker + "`n" + ($generatedContent -join "`n") + "`n" + $endMarker
	$finalContent = $templateContent -replace "(?s)$([regex]::Escape($startMarker)).*?$([regex]::Escape($endMarker))", $replacement
} else {
	Write-Host "ERROR: Required placeholder markers not found in template file!" -ForegroundColor Red
	Write-Host "Template file: $templatePath" -ForegroundColor Red
	Write-Host "Please add these markers to your template where you want the generated code:" -ForegroundColor Red
	Write-Host "  $startMarker" -ForegroundColor Yellow
	Write-Host "  $endMarker" -ForegroundColor Yellow
	exit 1
}

# Write the final content to output file
$finalContent | Out-File -FilePath $templatePath -Encoding UTF8

Write-Host "SUCCESS: Generated code written to $templatePath"
Write-Host "Generated $($systemPropertiesStructs.Count) struct entries"