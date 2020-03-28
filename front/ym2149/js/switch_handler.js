var baseUrl = "http://ym2149.local/api/v1/ym2149";
var currentEnvShape = "0000";
async function postData(url = '', data = {}) {
  // Default options are marked with *
  const response = await fetch(url, {
    method: 'POST', // *GET, POST, PUT, DELETE, etc.
    mode: 'cors', // no-cors, *cors, same-origin
    cache: 'no-cache', // *default, no-cache, reload, force-cache, only-if-cached
    credentials: 'same-origin', // include, *same-origin, omit
    headers: {
      'Content-Type': 'application/json'
      // 'Content-Type': 'application/x-www-form-urlencoded',
    },
    redirect: 'follow', // manual, *follow, error
    referrerPolicy: 'no-referrer', // no-referrer, *client
    body: JSON.stringify(data) // body data type must match "Content-Type" header
  });
  return await response.json(); // parses JSON response into native JavaScript objects
}

function channelNoiseSwitch(channel, isOn)
{
	var url = baseUrl + "/channel/noise";
	postData(url, { channel: channel, value: isOn })
		.then((data) => {
		console.log(data); // JSON data parsed by `response.json()` call
		});
	console.log();
		
}

function channelToneSwitch(channel, isOn)
{
	var url = baseUrl + "/channel/tone";
	postData(url, { channel: channel, value: isOn })
		.then((data) => {
		console.log(data); // JSON data parsed by `response.json()` call
		});
	console.log();
	
}

function channelAmpTypeSwitch(channel, isLvl)
{
	var url = baseUrl + "/channel/level/mode";
	postData(url, { channel: channel, value: isOn })
		.then((data) => {
		console.log(data); // JSON data parsed by `response.json()` call
		});
	console.log();
}

function channelMuteSwitch(channel, isOn)
{
	var url = baseUrl + "/channel/mute";
	postData(url, { channel: channel, value: isOn })
		.then((data) => {
		console.log(data); // JSON data parsed by `response.json()` call
		});
	console.log();
	
}

function envShapeSwitch(env_shape_type, isOn)
{
	changeSvg(env_shape_type, isOn);
	var url = baseUrl + "/envelope/shape";
	postData(url, { env_shape_type: env_shape_type, value: isOn })
		.then((data) => {
		console.log(data); // JSON data parsed by `response.json()` call
		});
	console.log();
	
}

function changeSvg(env_shape_type, isOn)
{	
	currentEnvShape = setCharAt(currentEnvShape,Math.abs(env_shape_type-3),isOn);
	console.log("currentEnvShape new:"+currentEnvShape);
	var svgFileName = "svg/env_shape_"+currentEnvShape+".svg";
	var clone=document.getElementById('svg_env').cloneNode(true);
	clone.setAttribute('data',svgFileName);
	document.getElementById('svg_env').parentNode.replaceChild(clone, document.getElementById('svg_env'));
	
}

function setCharAt(str,index,chr) {
    if(index > str.length-1) return str;
    return str.substr(0,index) + chr + str.substr(index+1);
}