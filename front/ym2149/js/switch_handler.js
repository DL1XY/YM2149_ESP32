function channelNoiseSwitch(channel, isOn)
{
	if (isOn == true)
		alert("Channel#" + channel +" Noise ON");
	else
		alert("Channel#" + channel +" Noise OFF");
		
}

function channelToneSwitch(channel, isOn)
{
	if (isOn == true)
		alert("Channel#" + channel +" Tone ON");
	else
		alert("Channel#" + channel +" Tone OFF");
	
}

function channelAmpTypeSwitch(channel, isLvl)
{
	if (isLvl == true)
		alert("Channel#" + channel +" Amp Lvl");
	else
		alert("Channel#" + channel +" Amp Env");
	
}

function channelMuteSwitch(channel, isOn)
{
	if (isOn == true)
		alert("Channel#" + channel +" Mute On");
	else
		alert("Channel#" + channel +" Mute Off");
	
}

function envShapeSwitch(fieldName, isOn)
{
	if (isOn == true)
		alert("Field " + fieldName +" is On");
	else
		alert("Field " + fieldName +" is Off");
	
}