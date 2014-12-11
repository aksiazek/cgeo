var SVG_CANVAS_SELECTOR = ".my-canvas";
var fileDataReader = null;
var inputDataReader = null;
var data = null;
var iterator = null;
var playing = false;
var onloadInitialization = function () {
};

var ScalingContext = {
    yMax: 0,
    yMin: 0,
    xMin: 0,
    xMax: 0,
    realWidth: 0,
    realHeight: 0,
    MARGIN: 50,
    AXES_MARGIN: 12,
    ZOOM_SPEED: 0.1,
    MOVE_SPEED: 0.1,
    zoom: function (speed) {
        var xdiff = this.xMax - this.xMin;
        this.xMax -= xdiff * speed;
        this.xMin += xdiff * speed;
        var ydiff = this.yMax - this.yMin;
        this.yMax -= ydiff * speed;
        this.yMin += ydiff * speed;
        drawSnapshot(iterator.getCurrent());
    },
    move: function (speedX, speedY) {
        var xdiff = this.xMax - this.xMin;
        this.xMax += xdiff * speedX;
        this.xMin += xdiff * speedX;
        var ydiff = this.yMax - this.yMin;
        this.yMax += ydiff * speedY;
        this.yMin += ydiff * speedY;
        drawSnapshot(iterator.getCurrent());
    },
    zoomIn: function () {
        this.zoom(this.ZOOM_SPEED);
    },
    zoomOut: function () {
        this.zoom(-this.ZOOM_SPEED);
    },
    moveLeft: function () {
        this.move(-this.MOVE_SPEED, 0);
    },
    moveRight: function () {
        this.move(this.MOVE_SPEED, 0);
    },
    moveDown: function () {
        this.move(0, -this.MOVE_SPEED);
    },
    moveUp: function () {
        this.move(0, this.MOVE_SPEED);
    },
    setRanges: function (_yMax, _yMin, _xMin, _xMax) {
        this.xMin = _xMin;
        this.xMax = _xMax;
        this.yMax = _yMax;
        this.yMin = _yMin;

        this.realWidth = $(SVG_CANVAS_SELECTOR).parent().width() - 2 * this.MARGIN;
        this.realHeight = $(SVG_CANVAS_SELECTOR).parent().height() - 2 * this.MARGIN;
    },
    fixupRangesToKeepRatio: function (svgs) {
        var dx = this.xMax - this.xMin;
        var dy = this.yMax - this.yMin;
        var xScale = dx / this.realWidth;
        var yScale = dy / this.realHeight;

        if (xScale > yScale) {
            var diff = dy * (xScale / yScale - 1);
            this.yMax = this.yMax + diff / 2;
            this.yMin = this.yMin - diff / 2;
        } else {
            var diff = dx * (yScale / xScale - 1);
            this.xMax = this.xMax + diff / 2;
            this.xMin = this.xMin - diff / 2;
        }
    },
    svgsXtoReal: function (svgs) {
        return (svgs - this.MARGIN) * (this.xMax - this.xMin) / this.realWidth + this.xMin;
    },
    svgsYtoReal: function (svgs) {
        return -1 * (svgs - this.MARGIN) * (this.yMax - this.yMin) / this.realHeight + this.yMax;
    },
    realXtoSvgs: function (_x) {
        var tmp = (_x - this.xMin) / (this.xMax - this.xMin);
        return (tmp * this.realWidth) + this.MARGIN;
    },
    realYtoSvgs: function (_y) {
        var tmp = ( this.yMax - _y) / (this.yMax - this.yMin);
        return (tmp * this.realHeight) + this.MARGIN;
    }
};


function initJS(filename) {
    var windowContext = this;
    var callbackFunction = function (_that) {
        _that.data = new DataRepository(_that.fileDataReader.getPoints(), _that.fileDataReader.getLines());
        _that.data.addSnapshots(_that.fileDataReader.getSnapshots());
        _that.iterator = new SnapshotsIterator(_that.data);
        _that.updateLabel();
        ScalingContext.setRanges(data.getYMax(), data.getYMin(), data.getXMin(), data.getXMax());
        ScalingContext.fixupRangesToKeepRatio();
    };
    fileDataReader = new FileDataReader(filename, callbackFunction, windowContext);
}

function appendYAxis(svgContainer) {
    var yAxisScale = d3.scale.linear()
        .domain([ScalingContext.yMax, ScalingContext.yMin])
        .range([ScalingContext.MARGIN, $(SVG_CANVAS_SELECTOR).height() - ScalingContext.MARGIN]);
    var yAxis = d3.svg.axis().scale(yAxisScale).orient("left");
    svgContainer.append("g")
        .attr("class", "axis axis-vertical")
        .attr("transform", "translate(" + (ScalingContext.MARGIN - ScalingContext.AXES_MARGIN) + ",0)")
        .call(yAxis);
}

function appendXAxis(svgContainer) {
    var xAxisScale = d3.scale.linear()
        .domain([ScalingContext.xMin, ScalingContext.xMax])
        .range([ScalingContext.MARGIN, $(SVG_CANVAS_SELECTOR).width() - ScalingContext.MARGIN]);
    var xAxis = d3.svg.axis().scale(xAxisScale).orient("bottom");
    svgContainer.append("g")
        .attr("class", "axis axis-horizontal")
        .attr("transform", "translate(0," + ($(SVG_CANVAS_SELECTOR).height() - ScalingContext.MARGIN + ScalingContext.AXES_MARGIN) + ")")
        .call(xAxis);
}

function registerFileInputHandler() {
    $("#upload").change(initWithFile);
}
function initWithFile() {
    var tmppath = URL.createObjectURL($("#upload").prop('files')[0]);
    initJS(tmppath);
}

function appendLine(realX1, realY1, realX2, realY2, lineID, style) {
    var x1 = ScalingContext.realXtoSvgs(realX1);
    var y1 = ScalingContext.realYtoSvgs(realY1);
    var x2 = ScalingContext.realXtoSvgs(realX2);
    var y2 = ScalingContext.realYtoSvgs(realY2);

    var lineHTMLTagOpen = "<line id=\"";
    var lineHTMLTagClose = "\" x1=\"0\" y1=\"0\" x2=\"0\" y2=\"0\" />";

    $(SVG_CANVAS_SELECTOR).append(lineHTMLTagOpen + lineID + lineHTMLTagClose);
    var tag = $(SVG_CANVAS_SELECTOR).children().last();

    tag.attr('x1', x1.toString());
    tag.attr('y1', y1.toString());
    tag.attr('x2', x2.toString());
    tag.attr('y2', y2.toString());
    tag.attr('realx1', realX1);
    tag.attr('realy1', realY1);
    tag.attr('realx2', realX2);
    tag.attr('realy2', realY2);

    tag.attr('gogui', 'line');
    tag.attr('style', style);
}

function appendPoint(realX, realY, pointID, style) {
    var x = ScalingContext.realXtoSvgs(realX);
    var y = ScalingContext.realYtoSvgs(realY);

    var pointHTMLTagOpen = "<circle id=\"";
    var pointHTMLTagClose = "\" cx=\"0\" cy=\"0\" r=\"0\" />";

    $(SVG_CANVAS_SELECTOR).append(pointHTMLTagOpen + pointID + pointHTMLTagClose);
    var tag = $(SVG_CANVAS_SELECTOR).children().last();

    tag.attr('cx', x.toString());
    tag.attr('cy', y.toString());
    tag.attr('r', "1");
    tag.attr('fill', '#000000');
    tag.attr('stroke-width', '3');
    tag.attr('realx', realX);
    tag.attr('realy', realY);
    tag.append('<title>(' + realX + ', ' + realY + ')</title>');
    tag.attr('gogui', 'point');
    tag.attr('style', style);

}

function drawSnapshot(snapshot) {
    $(SVG_CANVAS_SELECTOR).empty();

    var snapshotPoints = snapshot.getPoints();
    var snapshotLines = snapshot.getLines();

    // draw points
    for (var i = 0; i < snapshotPoints.length; ++i) {
        var p = data.getPointByIndex(snapshotPoints[i].pointID);
        var pointID = "point-" + i.toString();
        var style = "fill: " + snapshotPoints[i].color + "; stroke: " + snapshotPoints[i].color;
        appendPoint(p.x, p.y, pointID, style);
    }


    // draw lines
    for (var i = 0; i < snapshotLines.length; ++i) {
        var p1 = data.getPointByIndex(data.getLineByIndex(snapshotLines[i].lineID).p1);
        var p2 = data.getPointByIndex(data.getLineByIndex(snapshotLines[i].lineID).p2);
        var lineID = "line-" + i.toString();
        var style = "fill: " + snapshotLines[i].color + "; stroke: " + snapshotLines[i].color;
        appendLine(p1.x, p1.y, p2.x, p2.y, lineID, style);
    }

    var svgContainer = d3.select(SVG_CANVAS_SELECTOR);
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
    parseInput('{"history": [{"lines": [], "points": []}], "lines": [], "points": [{"x": 0, "y": 0},{"x": 1000, "y": 1000}]}');
    oneForward();
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
    $(SVG_CANVAS_SELECTOR).empty();
    iterator.reset();
    updateLabel();
}

function updateLabel() {
    $("#currentSnapshotInput").val(iterator.getSnapshotIndex());
    $("#maxSnapshotCount").text(data.getNumberOfSnapshots());
}

function stop() {
    playing = false;
}

function play() {
    animationPlayer();
    playing = true;
}

function view(snapshotNumber) {
    drawSnapshot(iterator.getByNumber(snapshotNumber));
}

function animationPlayer() {
    if (!iterator.hasNext()) {
        playing = false;
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

    data.addSnapshots(inputDataReader.getSnapshots());

    iterator = new SnapshotsIterator(data);
    iterator.reset();
    updateLabel();

    ScalingContext.setRanges(data.getYMax(), data.getYMin(), data.getXMin(), data.getXMax());
    ScalingContext.fixupRangesToKeepRatio();
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

