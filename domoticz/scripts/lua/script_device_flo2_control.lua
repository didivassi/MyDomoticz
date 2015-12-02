-- demo time script
-- script names have three name components: script_trigger_name.lua
-- trigger can be 'time' or 'device', name can be any string
-- domoticz will execute all time and device triggers when the relevant trigger occurs
--
-- copy this script and change the "name" part, all scripts named "demo" are ignored.
--
-- Make sure the encoding is UTF8 of the file
--
-- ingests tables: otherdevices,otherdevices_svalues
--
-- otherdevices and otherdevices_svalues are two item array for all devices:
--   otherdevices['yourotherdevicename']="On"
--	otherdevices_svalues['yourotherthermometer'] = string of svalues
--
-- Based on your logic, fill the commandArray with device commands. Device name is case sensitive.
--
-- Always, and I repeat ALWAYS start by checking for a state.
-- If you would only specify commandArray['AnotherDevice']='On', every time trigger (e.g. every minute) will switch AnotherDevice on.
--
-- The print command will output lua print statements to the domoticz log for debugging.
-- List all otherdevices states for debugging:
--   for i, v in pairs(otherdevices) do print(i, v) end
-- List all otherdevices svalues for debugging:
--   for i, v in pairs(otherdevices_svalues) do print(i, v) end

--
o2Switch='FL O2'
autoSwitch='Auto FL O2'
maxDifference=1800
maxAir=60
o2OnTime = '1' -- minutes for pump tobe on

t1 = os.time()
s = otherdevices_lastupdate[o2Switch]
-- returns a date time like 2013-07-11 17:23:12

year = string.sub(s, 1, 4)
month = string.sub(s, 6, 7)
day = string.sub(s, 9, 10)
hour = string.sub(s, 12, 13)
minutes = string.sub(s, 15, 16)
seconds = string.sub(s, 18, 19)

commandArray = {}

t2 = os.time{year=year, month=month, day=day, hour=hour, min=minutes, sec=seconds}
difference = (os.difftime (t1, t2))

if (otherdevices[o2Switch] == 'Off' and otherdevices[autoSwitch] == 'On' and difference > maxDifference) then
--	commandArray['SendNotification']='Refreshing '..'FL O2'..' for '..o2OnTime..' minutes'


	commandArray[autoSwitch]='Off' -- switch off after x minutes
	-- commandArray[autoSwitch]='Off FOR '..o2OnTime -- switch off after x minutes
	print(autoSwitch..' was turned Off for '..o2OnTime..' minutes')--to domoticz log

	commandArray[o2Switch]='On' -- switch off after x minutes
	-- commandArray[o2Switch]='On FOR '..o2OnTime -- switch off after x minutes
  print(o2Switch..' was turned On for '..o2OnTime..' minutes')--to domoticz log

elseif(otherdevices[o2Switch] == 'On' and otherdevices[autoSwitch] == 'Off' and difference > maxAir) then

	commandArray[autoSwitch]='On' -- switch off after x minutes
	-- commandArray[autoSwitch]='Off FOR '..o2OnTime -- switch off after x minutes
	print(autoSwitch..' was turned On after '..maxAir..' seconds')--to domoticz log

	commandArray[o2Switch]='Off' -- switch off after x minutes
	-- commandArray[o2Switch]='On FOR '..o2OnTime -- switch off after x minutes
  print(o2Switch..' was turned On after '..maxAir..' minutes')--to domoticz log

end
return commandArray
-- commandArray = {}
-- if (otherdevices['FL O2'] == 'Off' and otherdevices['Auto FL O2'] == 'On') then
-- 	commandArray['Auto FL O2']='Off FOR 2'
-- 		print('1')
-- 	commandArray['FL O2']='On FOR 2'
-- 	print('2')
-- end
--
-- return commandArray
