var fileDataReader = null;
var inputDataReader = null;
var data = null;
var iterator = null;
var playing = false;
var onloadInitialization = function () {
};

var ScalingContext = {
    yMax: null,
    yMin: null,
    xMin: null,
    xMax: null,
    MARGIN: 50,
    AXES_MARGIN: 12,
    svgsXtoReal: function (svgs) {
        var w = $(".canvas-area ").width() - 2 * this.MARGIN;
        return (svgs - this.MARGIN) * (this.xMax - this.xMin) / w + this.xMin;
    },
    svgsYtoReal: function (svgs) {
        var h = $(".canvas-area ").height() - 2 * this.MARGIN;
        return -1 * (svgs - this.MARGIN) * (this.yMax - this.yMin) / h + this.yMax;
    },
    realXtoSvgs: function (_x) {
        var tmp = (_x - this.xMin) / (this.xMax - this.xMin);
        var w = $(".canvas-area ").width() - 2 * this.MARGIN;
        return (tmp * w) + this.MARGIN;
    },
    realYtoSvgs: function (_y) {
        var tmp = ( this.yMax - _y) / (this.yMax - this.yMin);
        var h = $(".canvas-area ").height() - 2 * this.MARGIN;
        return (tmp * h) + this.MARGIN;
    }
};


function initJS(filename) {
    var windowContext = this;
    var callbackFunction = function (_that) {
        _that.data = new DataRepository(_that.fileDataReader.getPoints(), _that.fileDataReader.getLines());
        _that.data.addSnapshots(_that.fileDataReader.getSnapshots());
        _that.iterator = new SnapshotsIterator(_that.data);
        _that.updateLabel();
    };
    fileDataReader = new FileDataReader(filename, callbackFunction, windowContext);
}

function appendYAxis(svgContainer) {
    var yAxisScale = d3.scale.linear()
        .domain([ScalingContext.yMax, ScalingContext.yMin])
        .range([ScalingContext.MARGIN, $(".my-canvas").height() - ScalingContext.MARGIN]);
    var yAxis = d3.svg.axis().scale(yAxisScale).orient("left");
    svgContainer.append("g")
        .attr("class", "axis axis-vertical")
        .attr("transform", "translate(" + (ScalingContext.MARGIN - ScalingContext.AXES_MARGIN) + ",0)")
        .call(yAxis);
}

function appendXAxis(svgContainer) {
    var xAxisScale = d3.scale.linear()
        .domain([ScalingContext.xMin, ScalingContext.xMax])
        .range([ScalingContext.MARGIN, $(".my-canvas").width() - ScalingContext.MARGIN]);
    var xAxis = d3.svg.axis().scale(xAxisScale).orient("bottom");
    svgContainer.append("g")
        .attr("class", "axis axis-horizontal")
        .attr("transform", "translate(0," + ($(".my-canvas").height() - ScalingContext.MARGIN + ScalingContext.AXES_MARGIN) + ")")
        .call(xAxis);
}

function registerFileInputHandler() {
    $("#upload").change(initWithFile);
}
function initWithFile() {
    var tmppath = URL.createObjectURL($("#upload").prop('files')[0]);
    initJS(tmppath);
}

function appendLine(realX1, realY1, realX2, realY2, lineID, clazz) {
    var x1 = ScalingContext.realXtoSvgs(realX1);
    var y1 = ScalingContext.realYtoSvgs(realY1);
    var x2 = ScalingContext.realXtoSvgs(realX2);
    var y2 = ScalingContext.realYtoSvgs(realY2);

    var lineHTMLTagOpen = "<line id=\"";
    var lineHTMLTagClose = "\" x1=\"0\" y1=\"0\" x2=\"0\" y2=\"0\" />";

    $(".my-canvas").append(lineHTMLTagOpen + lineID + lineHTMLTagClose);
    var tag = $(".my-canvas").children().last();

    tag.attr('x1', x1.toString());
    tag.attr('y1', y1.toString());
    tag.attr('x2', x2.toString());
    tag.attr('y2', y2.toString());
    tag.attr('realx1', realX1);
    tag.attr('realy1', realY1);
    tag.attr('realx2', realX2);
    tag.attr('realy2', realY2);

    tag.attr('gogui', 'line');
    tag.attr('class', clazz);
}

function appendPoint(realX, realY, pointID, clazz) {
    var x = ScalingContext.realXtoSvgs(realX);
    var y = ScalingContext.realYtoSvgs(realY);

    var pointHTMLTagOpen = "<circle id=\"";
    var pointHTMLTagClose = "\" cx=\"0\" cy=\"0\" r=\"0\" />";

    $(".my-canvas").append(pointHTMLTagOpen + pointID + pointHTMLTagClose);
    var tag = $(".my-canvas").children().last();

    tag.attr('cx', x.toString());
    tag.attr('cy', y.toString());
    tag.attr('r', "1");
    tag.attr('fill', '#000000');
    tag.attr('stroke-width', '3');
    tag.attr('realx', realX);
    tag.attr('realy', realY);
    tag.append('<title>(' + realX + ', ' + realY + ')</title>');
    tag.attr('gogui', 'point');
    tag.attr('class', clazz);

}

function drawSnapshot(snapshot) {
    $(".my-canvas").empty();

    var snapshotPoints = snapshot.getPoints();
    var snapshotLines = snapshot.getLines();

    ScalingContext.yMax = data.getYMax();
    ScalingContext.yMin = data.getYMin();
    ScalingContext.xMin = data.getXMin();
    ScalingContext.xMax = data.getXMax();

    // draw points
    for (var i = 0; i < snapshotPoints.length; ++i) {
        var p = data.getPointByIndex(snapshotPoints[i].pointID);
        var pointID = "point-" + i.toString();
        if (snapshotPoints[i].style == "active") {
            clazz = 'active-point';
        } else if (snapshotPoints[i].style == "processed") {
            clazz = 'processed-point';
        } else {
            clazz = 'normal-point';
        }
        appendPoint(p.x, p.y, pointID, clazz);
    }


    // draw lines
    for (var i = 0; i < snapshotLines.length; ++i) {
        var p1 = data.getPointByIndex(data.getLineByIndex(snapshotLines[i].lineID).p1);
        var p2 = data.getPointByIndex(data.getLineByIndex(snapshotLines[i].lineID).p2);
        var lineID = "line-" + i.toString();
        if (snapshotLines[i].style == "normal") {
            clazz = 'normal-line';
        } else if (snapshotLines[i].style == "processed") {
            clazz = 'processed-line';
        } else {
            clazz = 'active-line';
        }
        appendLine(p1.x, p1.y, p2.x, p2.y, lineID, clazz);
    }

    var svgContainer = d3.select(".my-canvas");
    appendXAxis(svgContainer);
    appendYAxis(svgContainer);

    /* WHY?! */
    $("body").html($("body").html());

    onloadInitialization();
}

function getDisplayedPoints() {
    return $.map($('[gogui="point"]'), function (x) {
        var point = $(x);
        return {'x': parseFloat(point.attr('realx')), 'y': parseFloat(point.attr('realy'))};
    })
}

function getDisplayedLines() {
    return $.map($('[gogui="line"]'), function (x) {
        var point = $(x);
        return {
            'start': {'x': parseFloat(point.attr('realx1')), 'y': parseFloat(point.attr('realy1'))},
            'end': {'x': parseFloat(point.attr('realx2')), 'y': parseFloat(point.attr('realy2'))}
        }
    })
}

function getBlankCanvas() {
    parseInput('{"history": [{"lines": [], "points": []}], "lines": [], "points": [{"x": 1000, "y": 1000}]}');
}

function getPointsAsText() {
    var points = getDisplayedPoints();
    var result = '' + points.length + '\n';
    var textLines = $.map(points, function (point) {
        return '' + point.x + ' ' + point.y;
    });
    result += textLines.join('\n');
    return result;
}

function getLinesAsText() {
    var lines = getDisplayedLines();
    var result = '' + lines.length + '\n';
    var textLines = $.map(lines, function (line) {
        return '' + line.start.x + ' ' + line.start.y + ' ' + line.end.x + ' ' + line.end.y;
    });
    result += textLines.join('\n');
    return result;
}

function oneForward() {
    if (!iterator.hasNext()) {
        $('#forwardButton').attr("disabled", true);
    } else {
        drawSnapshot(iterator.getNext());
    }
    updateLabel();
}

function oneBackward() {
    if (!iterator.hasPrevious()) {
        $('#backwardButton').attr("disabled", true);
    } else {
        drawSnapshot(iterator.getPrevious());
    }
    updateLabel();
}

function reset() {
    $(".my-canvas").empty();
    iterator.reset();
    updateLabel();
}

function updateLabel() {
    $(".counterLabel").empty();
    var labelText = "";
    labelText = "" + ( iterator.getSnapshotIndex() + 1 ).toString() + " / " + data.getNumberOfSnapshots().toString();
    $(".counterLabel").append(labelText);
}

function play() {
    if (playing) {
        $("#playButton").html('Odtwarzaj');
    } else {
        $("#playButton").html('Zatrzymaj');
        animationPlayer();
    }
    playing = !playing;
}

function animationPlayer() {
    if (!iterator.hasNext()) {
        playing = false;
        $("#playButton").html('Odtwarzaj');
        return;
    } else {
        setTimeout(function () {
            if (playing) {
                oneForward();
                animationPlayer();
            }
        }, 100);
    }
}

function parseInput(text) {
    inputDataReader = new InputDataReader(text);
    data = new DataRepository(inputDataReader.getPoints(), inputDataReader.getLines());

    setTimeout(function () {
        data.addSnapshots(inputDataReader.getSnapshots());
        updateLabel();
    }, 150);

    iterator = new SnapshotsIterator(data);
    iterator.reset();
    updateLabel();
}


if (window.attachEvent) {   // just repeating last step
    window.attachEvent('onresize', function () {
        iterator.getPrevious();
        oneForward();
    });
}
else if (window.addEventListener) {
    window.addEventListener('resize', function () {
        iterator.getPrevious();
        oneForward();
    }, true);
}

