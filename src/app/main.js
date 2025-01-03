var fs = require('fs');
var http = require('http');
const URL =  require("url")
const socket = require("../helpers/websocket")
const {google} = require("googleapis")
const {CLIENT_ID, SECRET,REDIRECT} = require("../configs/index")

const oAuthClient = new google.auth.OAuth2(CLIENT_ID, SECRET, REDIRECT)

const indexPage = fs.readFileSync(`./src/templates/index.html`, "utf8")
const mainPage = fs.readFileSync(`./src/templates/main-page.html`, "utf8")

const schedules = {}

const handleGoogleLogin = (response) => {
    console.log("check")
    const url = oAuthClient.generateAuthUrl({
        access_type: "offline",
        scope: "https://www.googleapis.com/auth/calendar.readonly"
    })

    response.writeHead(302, {
        'Location': url,
    });
    response.end();
}

const handleGetEvents = (parsedUrl, response) => {
    const queryParams = parsedUrl.query;
    const calendarId = queryParams.calendar ?? 'primary';
    // Create a Google Calendar API client
    const calendar = google.calendar({ version: 'v3', auth: oAuthClient });
    // List events from the specified calendar
    calendar.events.list({
      calendarId,
      timeMin: (new Date()).toISOString(),
      maxResults: 15,
      singleEvents: true,
      orderBy: 'startTime'
    }, (err, res) => {
      if (err) {
        console.error('Can\'t fetch events', err);
        response.end('<h1>Error while fetching calendar</h1>');
        return;
      }
  
      const events = res.data.items;

      return response.end(JSON.stringify(events));
    });
}

function fetchEventsAndHandleBoardcast() {
    console.log("re run this shit")
    const calendarId = 'primary';
    const calendar = google.calendar({ version: 'v3', auth: oAuthClient });

    calendar.events.list({
      calendarId,
      timeMin: (new Date()).toISOString(),
      maxResults: 15,
      singleEvents: true,
      orderBy: 'startTime'
    }, (err, res) => {
      if (err) {
        console.error('Can\'t fetch events', err);
        return;
      }
      const events = res.data.items;

      handleBoardcastEvent(events)
    });
}

function calculateDateDiffToMs(date, timeZone) {
    const currentDate = new Date(new Date().toLocaleString("en-US", {timeZone: timeZone}))
    const _date = new Date(date)

    return Math.ceil((_date - currentDate));
}

function handleBoardcastEvent(events) {
    const inExpiredEvents = events.filter(e => new Date(e.end.dateTime) > new Date())

    console.log({events})
    socket.broadcastAll(`EVENT_UPDATE - ${inExpiredEvents
        // .filter(e => calculateDateDiffToMs(e.start.dateTime, e.start.timeZone) > 86_400) 
        .map(e => new Date(e.start.dateTime).getHours()).join(" - ")}
    `)

    inExpiredEvents.forEach((e) => {
        const startDateGap = calculateDateDiffToMs(e.start.dateTime, e.start.timeZone)
        const eventEndGap = calculateDateDiffToMs(e.end.dateTime, e.end.timeZone)

        if (startDateGap < 0 && eventEndGap > 0) {
            handleEventOnGoing(e)
        } else if (startDateGap > 0) {
            handleEventNotStartedYet(e)
        } 
     })
}

const EVENT_MESSAGES = {
    occur: "EVENT_OCCUR",
    end: 'EVENT_END'
}

function handleEventNotStartedYet(e) {
    const eventStartDate = new Date(e.start.dateTime)
    const eventEndDate = new Date(e.end.dateTime)
    const timeStartDiff = calculateDateDiffToMs(eventStartDate, e.start.timeZone)
    const timeEndDiff = calculateDateDiffToMs(eventEndDate, e.end.timeZone)

    if (timeStartDiff > 86_400) {
        return;
    }

    if(schedules[e.id]) {
        return;
    }

    console.log("set", e.id)
 
    setTimeout(() => {
        socket.broadcastAll(`${EVENT_MESSAGES.occur} - ${e.id}`)
    }, timeStartDiff)

    setTimeout(() => {
        socket.broadcastAll(`${EVENT_MESSAGES.end} - ${e.id}`)
        delete schedules[e.id]
        console.log("del", e.id)
    }, timeEndDiff)

    schedules[e.id] = true
}

function handleEventOnGoing(e) {
    if(schedules[e.id]) {
        return;
    }
    
    console.log("set", e.id)

    const eventEndDate = new Date(e.end.dateTime)

    const timeEndDiff =  calculateDateDiffToMs(eventEndDate,e.end.timeZone )

    socket.broadcastAll(`${EVENT_MESSAGES.occur} - ${e.id}`)

    setTimeout(() => {
        socket.broadcastAll(`${EVENT_MESSAGES.end} - ${e.id}`)
        delete schedules[e.id]
        console.log("del", e.id)
    }, timeEndDiff)

    schedules[e.id] = true
}

const handleRedirect = (parsedUrl, response) => {
    const queryParams = parsedUrl.query;
        const code = queryParams.code

    oAuthClient.getToken(code, (err, token) => {
        if(err) {
            return response.end("<h1>Authentication failed</h1>");
        }

        oAuthClient.setCredentials(token)

        setInterval(fetchEventsAndHandleBoardcast, 5000)

        response.end(mainPage);
    })
}


function requestHandler(request, response) {
    const url = request.url;
    const parsedUrl = URL.parse(url, true);
    const pathname = parsedUrl.pathname

    response.setHeader('Access-Control-Allow-Origin', '*');
    response.setHeader('Access-Control-Allow-Methods', 'GET, POST, PUT, DELETE');
    response.setHeader('Access-Control-Allow-Headers', 'Content-Type, Authorization');

    switch(pathname) {
        case "/":
            return response.end(indexPage);
        case "/login":
            return handleGoogleLogin(response)
        case "/redirect":
            return handleRedirect(parsedUrl, response);
        case "/events":
            return handleGetEvents(parsedUrl, response);
    }
}

(async () => {
    var server = http.createServer(requestHandler);

    socket.connectSocket(server)
    
    server.listen(3000, () => {
        console.log("Server listen on 3000")
    });
})()