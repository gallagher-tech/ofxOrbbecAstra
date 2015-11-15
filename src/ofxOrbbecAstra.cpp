//
//  ofxOrbbecAstra.h
//  ofxOrbbecAstra
//
//  Created by Matt Felsen on 10/24/15.
//
//

#include "ofxOrbbecAstra.h"

ofxOrbbecAstra::ofxOrbbecAstra() :
    streamset(nullptr),
    reader(nullptr)
{}

ofxOrbbecAstra::~ofxOrbbecAstra(){
    astra::Astra::terminate();
}

void ofxOrbbecAstra::setup(){
    int width = 640;
    int height = 480;
    colorImg.allocate(width, height, OF_IMAGE_COLOR);
    depthImg.allocate(width, height, OF_IMAGE_GRAYSCALE);

    astra::Astra::initialize();

    streamset = make_unique<astra::StreamSet>();
    reader = make_unique<astra::StreamReader>(streamset->create_reader());

    // set up color stream
    astra::ImageStreamMode colorMode;
    auto colorStream = reader->stream<astra::ColorStream>();

    colorMode.set_width(width);
    colorMode.set_height(height);
    colorMode.set_pixelFormat(astra_pixel_formats::ASTRA_PIXEL_FORMAT_RGB888);
    colorMode.set_fps(30);

    colorStream.set_mode(colorMode);
    colorStream.start();

    // set up depth stream
    astra::ImageStreamMode depthMode;
    auto depthStream = reader->stream<astra::DepthStream>();

    depthMode.set_width(640);
    depthMode.set_height(480);
    depthMode.set_pixelFormat(astra_pixel_formats::ASTRA_PIXEL_FORMAT_DEPTH_MM);
    depthMode.set_fps(30);

    depthStream.set_mode(depthMode);
    depthStream.start();

    reader->addListener(*this);
}

void ofxOrbbecAstra::update(){
    // See on_frame_ready() for more processing
    astra_temp_update();
}

void ofxOrbbecAstra::draw(){
    colorImg.draw(0, 0);
    depthImg.draw(640, 0);
}

void ofxOrbbecAstra::on_frame_ready(astra::StreamReader& reader,
                                    astra::Frame& frame)
{
    astra::ColorFrame colorFrame = frame.get<astra::ColorFrame>();
    astra::DepthFrame depthFrame = frame.get<astra::DepthFrame>();

    if (colorFrame.is_valid()) {
        const astra::RGBPixel* colorData = colorFrame.data();
        colorFrame.copy_to((astra::RGBPixel*) colorImg.getPixels().getData());
        colorImg.update();
    }

    if (depthFrame.is_valid()) {
        const short* depthData = depthFrame.data();
        depthFrame.copy_to((short*) depthImg.getPixels().getData());

        for (int i = 0; i < depthImg.getPixels().size(); i++) {
            short depth = depthImg.getColor(i).r;
            float val = (depth != 0) ? ofMap(depth, 1800, 300, 0, 255, true) : 0;
            depthImg.setColor(i, ofColor(val));
        }
        
        depthImg.update();
    }
}