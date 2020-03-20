
var knobColor = '#00ccff';

var baseUrl = "http://ym2149.local/api/v1/ym2149";

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

function getChannelId(channel)
{
	if (channel == 'A') return 1;
	if (channel == 'B') return 2;
	if (channel == 'C') return 3;

}
function channelRoughKnob(channel) {
	// Create knob element, 300 x 300 px in size.
	const knob = pureknob.createKnob(128, 128);

	// Set properties.
	knob.setProperty('angleStart', -0.75 * Math.PI);
	knob.setProperty('angleEnd', 0.75 * Math.PI);
	knob.setProperty('channel', getChannelId(channel));
	knob.setProperty('colorFG', knobColor);
	knob.setProperty('trackWidth', 0.4);
	knob.setProperty('valMin', 0);
	knob.setProperty('valMax', 15);

	// Set initial value.
	knob.setValue(0);

	/*
	 * Event listener.
	 *
	 * Parameter 'knob' is the knob object which was
	 * actuated. Allows you to associate data with
	 * it to discern which of your knobs was actuated.
	 *
	 * Parameter 'value' is the value which was set
	 * by the user.
	 */
	const listener = function(knob, value) {
		var url = baseUrl + "/channel/freq/rough";
		postData(url, { channel: knob.getProperty('channel'), value: value })
		  .then((data) => {
		    console.log(data); // JSON data parsed by `response.json()` call
		  });
		console.log();
	};

	knob.addListener(listener);

	// Create element node.
	const node = knob.node();

	// Add it to the DOM.
	const elem = document.getElementById('channel_'+channel.toLowerCase()+'_rough_tune');
	elem.appendChild(node);
}

function channelFineKnob(channel) {
	// Create knob element, 300 x 300 px in size.
	const knob = pureknob.createKnob(128, 128);

	// Set properties.
	knob.setProperty('angleStart', -0.75 * Math.PI);
	knob.setProperty('angleEnd', 0.75 * Math.PI);
	knob.setProperty('colorFG', knobColor);
	knob.setProperty('trackWidth', 0.4);
	knob.setProperty('valMin', 0);
	knob.setProperty('valMax', 255);

	// Set initial value.
	knob.setValue(0);

	/*
	 * Event listener.
	 *
	 * Parameter 'knob' is the knob object which was
	 * actuated. Allows you to associate data with
	 * it to discern which of your knobs was actuated.
	 *
	 * Parameter 'value' is the value which was set
	 * by the user.
	 */
	const listener = function(knob, value) {
		console.log(value);
	};

	knob.addListener(listener);

	// Create element node.
	const node = knob.node();

	// Add it to the DOM.
	const elem = document.getElementById('channel_'+channel.toLowerCase()+'_fine_tune');
	elem.appendChild(node);
}

function channelLevelKnob(channel) {
	// Create knob element, 300 x 300 px in size.
	const knob = pureknob.createKnob(128, 128);

	// Set properties.
	knob.setProperty('angleStart', -0.75 * Math.PI);
	knob.setProperty('angleEnd', 0.75 * Math.PI);
	knob.setProperty('colorFG', knobColor);
	knob.setProperty('trackWidth', 0.4);
	knob.setProperty('valMin', 0);
	knob.setProperty('valMax', 255);

	// Set initial value.
	knob.setValue(0);

	/*
	 * Event listener.
	 *
	 * Parameter 'knob' is the knob object which was
	 * actuated. Allows you to associate data with
	 * it to discern which of your knobs was actuated.
	 *
	 * Parameter 'value' is the value which was set
	 * by the user.
	 */
	const listener = function(knob, value) {
		console.log(value);
	};

	knob.addListener(listener);

	// Create element node.
	const node = knob.node();

	// Add it to the DOM.
	const elem = document.getElementById('channel_'+channel.toLowerCase()+'_level');
	elem.appendChild(node);
}

function envelopeFineKnob() {
	// Create knob element, 300 x 300 px in size.
	const knob = pureknob.createKnob(128, 128);

	// Set properties.
	knob.setProperty('angleStart', -0.75 * Math.PI);
	knob.setProperty('angleEnd', 0.75 * Math.PI);
	knob.setProperty('colorFG', knobColor);
	knob.setProperty('trackWidth', 0.4);
	knob.setProperty('valMin', 0);
	knob.setProperty('valMax', 255);

	// Set initial value.
	knob.setValue(0);

	/*
	 * Event listener.
	 *
	 * Parameter 'knob' is the knob object which was
	 * actuated. Allows you to associate data with
	 * it to discern which of your knobs was actuated.
	 *
	 * Parameter 'value' is the value which was set
	 * by the user.
	 */
	const listener = function(knob, value) {
		console.log(value);
	};

	knob.addListener(listener);

	// Create element node.
	const node = knob.node();

	// Add it to the DOM.
	const elem = document.getElementById('envelope_fine_frequency');
	elem.appendChild(node);
}

function envelopeRoughKnob() {
	// Create knob element, 300 x 300 px in size.
	const knob = pureknob.createKnob(128, 128);

	// Set properties.
	knob.setProperty('angleStart', -0.75 * Math.PI);
	knob.setProperty('angleEnd', 0.75 * Math.PI);
	knob.setProperty('colorFG', knobColor);
	knob.setProperty('trackWidth', 0.4);
	knob.setProperty('valMin', 0);
	knob.setProperty('valMax', 255);

	// Set initial value.
	knob.setValue(0);

	/*
	 * Event listener.
	 *
	 * Parameter 'knob' is the knob object which was
	 * actuated. Allows you to associate data with
	 * it to discern which of your knobs was actuated.
	 *
	 * Parameter 'value' is the value which was set
	 * by the user.
	 */
	const listener = function(knob, value) {
		console.log(value);
	};

	knob.addListener(listener);

	// Create element node.
	const node = knob.node();

	// Add it to the DOM.
	const elem = document.getElementById('envelope_rough_frequency');
	elem.appendChild(node);
}

function noiseFreqKnob() {
	// Create knob element, 300 x 300 px in size.
	const knob = pureknob.createKnob(128, 128);

	// Set properties.
	knob.setProperty('angleStart', -0.75 * Math.PI);
	knob.setProperty('angleEnd', 0.75 * Math.PI);
	knob.setProperty('colorFG', knobColor);
	knob.setProperty('trackWidth', 0.4);
	knob.setProperty('valMin', 0);
	knob.setProperty('valMax', 15);

	// Set initial value.
	knob.setValue(0);

	/*
	 * Event listener.
	 *
	 * Parameter 'knob' is the knob object which was
	 * actuated. Allows you to associate data with
	 * it to discern which of your knobs was actuated.
	 *
	 * Parameter 'value' is the value which was set
	 * by the user.
	 */
	const listener = function(knob, value) {
		console.log(value);
	};

	knob.addListener(listener);

	// Create element node.
	const node = knob.node();

	// Add it to the DOM.
	const elem = document.getElementById('noise_frequency');
	elem.appendChild(node);
}