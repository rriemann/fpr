#!/usr/bin/ruby
# kate: remove-trailing-space on; replace-trailing-space-save on; indent-width 2; indent-mode ruby; syntax ruby;
# this script requires as arguments an output dir and at least one readable root file.
# usage: ./diagram.rb folder file.root [file2.root ...]

$KCODE = 'UTF8'

begin
  require 'libRuby'
  gROOT.SetBatch true
rescue LoadError
  puts "please initialize the ruby root environment variables first."
  exit 1
end
require 'fileutils'

HistStruct = Struct.new(:file, :histogram, :title) # everthing is a string, file can be a TFile

module Diagram
  # create an Array containing the colors for TH1F::SetLineColor(int)
  COLORS = [1, 2, 4]
  SIZE = [800, 600]
  OUTPUTFOLDER = "./plots"

  def self.setup_style
    # based on a style file from BaBar
    atlasStyle = ::TStyle.new("ATLAS","Atlas style")

    # use plain black on white colors
    icol=0
    atlasStyle.SetFrameBorderMode(icol)
    atlasStyle.SetCanvasBorderMode(icol)
    atlasStyle.SetPadBorderMode(icol)
    atlasStyle.SetPadColor(icol)
    atlasStyle.SetCanvasColor(icol)
    atlasStyle.SetStatColor(icol)
    atlasStyle.SetFrameFillColor(icol)
    # atlasStyle.SetFillColor(icol)

    nRGBs = 5
    nCont = 255
    stops = [ 0.00, 0.34, 0.61, 0.84, 1.00 ]
    red   = [ 0.00, 0.00, 0.87, 1.00, 0.51 ]
    green = [ 0.00, 0.81, 1.00, 0.20, 0.00 ]
    blue  = [ 0.51, 1.00, 0.12, 0.00, 0.00 ]

    ::TColor.CreateGradientColorTable(nRGBs, stops, red, green, blue, nCont)
    gStyle.SetNumberContours(nCont)
    atlasStyle.SetNumberContours(nCont)

    # set the paper & margin sizes
    atlasStyle.SetPaperSize(20,26)
    atlasStyle.SetPadTopMargin(0.05)
    atlasStyle.SetPadRightMargin(0.05)
    atlasStyle.SetPadBottomMargin(0.16)
    atlasStyle.SetPadLeftMargin(0.12)

    # use large fonts
    # Int_t font=72
    font=42
    tsize=0.05
    atlasStyle.SetTextFont(font)

    atlasStyle.SetTextSize(tsize)
    atlasStyle.SetLabelFont(font,"x")
    atlasStyle.SetTitleFont(font,"x")
    atlasStyle.SetLabelFont(font,"y")
    atlasStyle.SetTitleFont(font,"y")
    atlasStyle.SetLabelFont(font,"z")
    atlasStyle.SetTitleFont(font,"z")

    atlasStyle.SetLabelSize(tsize,"x")
    atlasStyle.SetTitleSize(tsize,"x")
    atlasStyle.SetLabelSize(tsize,"y")
    atlasStyle.SetTitleSize(tsize,"y")
    atlasStyle.SetLabelSize(tsize,"z")
    atlasStyle.SetTitleSize(tsize,"z")

    # use bold lines and markers
    atlasStyle.SetMarkerStyle(20)
    atlasStyle.SetMarkerSize(1.0) # standard was 1.2
    atlasStyle.SetHistLineWidth(2.0)
    atlasStyle.SetLineStyleString(2,"[12 12]") # postscript dashes

    #get rid of X error bars and y error bar caps
    # atlasStyle.SetErrorX(0.001)

    # do not display any of the standard histogram decorations
    atlasStyle.SetOptTitle(0)
    # atlasStyle.SetOptStat(1111)
    atlasStyle.SetOptStat(0)
    # atlasStyle.SetOptFit(1111)
    # atlasStyle.SetOptFit(0)

    # put tick marks on top and RHS of plots
    atlasStyle.SetPadTickX(1)
    atlasStyle.SetPadTickY(1)

    ################ custom

    # gROOT.Reset()
    # gStyle.SetCanvasColor(10)
    # gStyle.SetPalette(1)
    gStyle.SetPaintTextFormat("3.1f")
    # overflow, underflow, rms, mean, count of enries
    atlasStyle.SetOptStat("ourme")

    gROOT.SetStyle("ATLAS")
    # gStyle.SetMarkerSize(.5)
    # gROOT.ForceStyle
  end

  def self.create_histogram_index file, hist_names = [], prefix = []
    if prefix.empty?
      list = file.GetListOfKeys
    else
      list = file.Get( prefix.join("/") ).GetListOfKeys
    end
    (0 .. list.LastIndex).each do |index|
      item = list.At(index)
      if item.IsFolder
        create_histogram_index file, hist_names, prefix + [item.GetName]
      else
        hist_names.push( (prefix + [item.GetName]).join("/") )
      end
    end
    hist_names
  end

  def self.create_histogram histograms, dataOutputFileName, size = SIZE, normalize = true
    canvas = ::TCanvas.new dataOutputFileName, "", *size
    canvas.SetFillColor 0 # set background color to white
    options = []

    dimension = histograms[0].GetDimension
    if dimension == 1
      legend = ::TLegend.new 0.02, 0.02, 0.6, 0.11
      legend.SetBorderSize(1)
      legend.SetFillColor(0)
    elsif dimension > 1
      options.push "COLZ"
      canvas.Divide histograms.size, 1 # TODO create a grid
    end

    histograms.each_with_index do |histogram,i|
      if dimension == 1
        histogram.Scale 1/histogram.Integral if normalize
        # http://root.cern.ch/root/roottalk/roottalk01/0359.html
        # histogram.SetBinContent(1, histogram.GetBinContent(0) )
        # histogram.SetBinContent(histogram.GetSize - 2, histogram.GetBinContent( histogram.GetSize - 1) )
        histogram.SetLineColor COLORS[i]
        histogram.SetMarkerStyle 20 # use little circles
        histogram.SetMarkerColor COLORS[i]
        legend.AddEntry histogram, histogram.GetTitle, "lpf"
      elsif dimension > 1
        canvas.cd i + 1 # first index has to be 1
      end
      histogram.Draw options.join( "," )
      if i == 0 and dimension == 1 # print following histograms in the same canvas
        options.push "SAME"
      end
    end
    legend.Draw if dimension == 1
    dir = File.dirname dataOutputFileName
    FileUtils.mkdir_p dir unless File.directory? dir
    canvas.SaveAs dataOutputFileName
  end

  def self.create_all_histograms files, dataOutputFolder = OUTPUTFOLDER, dataOutputFileNameSuffix = ".png", size = SIZE, normalize = true
    create_histogram_index( files[0] ).each do |hist_name|
      dataOutputFileName = dataOutputFolder + '/' + hist_name + dataOutputFileNameSuffix

      histograms = []
      # loop over all files
      files.each do |file|
        histogram = file.Get hist_name
        if histogram
          histogram.SetTitle "#{histogram.GetTitle} (#{file.GetName})"
          histograms.push histogram
        else
          $stderr.puts "not found #{hist_name} in #{file.GetName}"
        end
      end

      create_histogram histograms, dataOutputFileName, size, normalize
    end
  end

  def self.create_from_stringstruct histogram_structs, dataOutputFolder = OUTPUTFOLDER
    histograms = []
    histogram_structs.each do |struct|
      if struct.file.class == TFile
        file = struct.file
      else
        file = TFile.new struct.file
      end
      raise "#{struct.file} is a Zombie" if file.IsZombie
      histogram = file.Get struct.histogram
      histogram.SetTitle = struct.title unless struct.title.nil?
      histograms.push histogram
    end
    create_histogram histograms, dataOutputFolder
  end

end

raise "please provide at least one output path and one root file as arguments." if ARGV.size < 2
dataOutputFolder = ARGV.shift                                           #1. arg: foldername; andere arg.: root-files. shift -> arg. eins nach links, 1. in die var. getan.

# create an Array dataFiles which contains valid ROOT TFile objects
dataFiles = ARGV.collect do |dataFileName|
  file = TFile.new dataFileName
  if file.IsZombie
    nil
  else
    file
  end
end
raise "some files are declared as Zombies - plz check." if dataFiles.include? nil

# puts diagram.hist_names.inspect
Diagram.setup_style
Diagram.create_all_histograms dataFiles, dataOutputFolder, ".png", [1000,600], false


