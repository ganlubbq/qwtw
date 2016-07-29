module qwtw

qwtwLibHandle = 0
qwtwFigureH = 0
qwtwTopviewH = 0
qwtwModeH = 0
qwtwsetimpstatusH = 0
qwtwCLearH = 0
qwtwPlotH = 0
qwtwPlot2H = 0
qwtwXlabelH = 0
qwtwYlabelH = 0
qwywTitleH = 0
qwtwVersionH = 0
qwtwMWShowH = 0

function qwtwStart()
	libName = @unix ? "libqwtwc.so" : "qwtwc"
	global qwtwLibHandle, qwtwFigureH, qwtwTopviewH, qwtwModeH, qwtwsetimpstatusH, qwtwCLearH, qwtwPlotH
	global qwtwPlot2H, qwtwXlabelH, qwtwYlabelH, qwywTitleH, qwtwVersionH, qwtwMWShowH

	if qwtwLibHandle != 0 # looks like we already started
		return
	end
	qwtwStop()
	qwtwLibHandle = Libdl.dlopen(libName)
	qwtwFigureH = Libdl.dlsym(qwtwLibHandle, "qwtfigure")
	qwtwTopviewH = Libdl.dlsym(qwtwLibHandle, "topview")
	qwtwModeH = Libdl.dlsym(qwtwLibHandle, "qwtmode")
	qwtwsetimpstatusH = Libdl.dlsym(qwtwLibHandle, "qwtsetimpstatus")
	qwtwCLearH = Libdl.dlsym(qwtwLibHandle, "qwtclear")
	qwtwPlotH = Libdl.dlsym(qwtwLibHandle, "qwtplot")
	qwtwPlot2H = Libdl.dlsym(qwtwLibHandle, "qwtplot2")
	qwtwXlabelH = Libdl.dlsym(qwtwLibHandle, "qwtxlabel")
	qwtwYlabelH = Libdl.dlsym(qwtwLibHandle, "qwtylabel")
	qwywTitleH = Libdl.dlsym(qwtwLibHandle, "qwttitle")
	qwtwVersionH = Libdl.dlsym(qwtwLibHandle, "qwtversion")
	qwtwMWShowH = Libdl.dlsym(qwtwLibHandle, "qwtshowmw")


	version = qversion()
	println(version)

	return
end

function qwtwStop()
	global qwtwLibHandle
	if qwtwLibHandle != 0
		Libdl.dlclose(qwtwLibHandle)
	end
	qwtwLibHandle = 0
end

macro qLibName()
	libName = @unix ? "libqwtwc.so" : "qwtwc"
	return (libName)
end

# return version info (as string)
function qversion()
	global qwtwVersionH
	v =  Array(Int8, 128)
	ccall(qwtwVersionH, Int32, (Ptr{Int8},), v);
	return bytestring(pointer(v))
end;

function traceit( msg )
      global g_bTraceOn

      if ( true )
         bt = backtrace() ;
         s = sprint(io->Base.show_backtrace(io, bt))

         println( "debug: $s: $msg" )
      end
end

function qfigure(n)
#  a::ASCIIString = libName();
	global qwtwFigureH
	ccall(qwtwFigureH, Void, (Int32,), n);
end;

function qfmap(n)
	global qwtwTopviewH
	ccall(qwtwTopviewH, Void, (Int32,), n);
end;

# 2 - normal lines
#3  - top view lines
function qsetmode(m)
	global qwtwModeH
#  a::ASCIIString = libName();
	ccall(qwtwModeH, Void, (Int32,), m);
end;

#=  set up an importance status for next lines. looks like '0' means 'not important'

'not important' will not participate in 'clipping'
=#
function qimportant(i)
	global qwtwsetimpstatusH
	ccall(qwtwsetimpstatusH, Void, (Int32,), i);
end

#= close all the plots

=#
function qclear()
	global qwtwCLearH
	ccall(qwtwCLearH, Void, ());
end

function qsmw()
	global qwtwMWShowH
	ccall(qwtwMWShowH, Void, ());
end

# plot normal lines
function qplot(x::Vector{Float64}, y::Vector{Float64}, name::ASCIIString, style::ASCIIString, w)
#function qplot(x::Vector{Any}, y::Vector{Any}, name::ASCIIString, style::ASCIIString, w::Int64)
	global qwtwPlotH
	if length(x) != length(y)
		@printf("qplot: x[%d], y[%d]\n", length(x), length(y))
		traceit("error")
	end
	assert(length(x) == length(y))

	n = length(x)
	ww::Int32 = w;
	try
		ccall(qwtwPlotH, Void, (Ptr{Float64}, Ptr{Float64}, Int32, Ptr{Uint8}, Ptr{Uint8}, Int32, Int32),
			x, y, n, name, style, ww, 1);
		sleep(0.025)
	catch
		@printf("qplot: error #2\n")
		traceit("error #2")
	end

end;

# draw symbols
function qplot1(x::Vector{Float64}, y::Vector{Float64}, name::ASCIIString, style::ASCIIString, w)
	global qwtwPlotH
	assert(length(x) == length(y))
	n = length(x)
	ww::Int32 = w;
	ccall(qwtwPlotH, Void, (Ptr{Float64}, Ptr{Float64}, Int32, Ptr{Uint8}, Ptr{Uint8}, Int32, Int32),
		x, y, n, name, style, 1, ww);
	sleep(0.025)

end;


# plot 'top view'
function qplot2(x::Array{Float64}, y::Array{Float64}, name::ASCIIString, style::ASCIIString, w, time::Array{Float64})
	global qwtwPlot2H
	assert(length(x) == length(y))
	n = length(x)
	ww::Int32 = w;
	ccall(qwtwPlot2H, Void, (Ptr{Float64}, Ptr{Float64}, Int32, Ptr{Uint8}, Ptr{Uint8}, Int32, Int32, Ptr{Float64}),
		x, y, n, name, style, ww, 1, time);
	sleep(0.025)

end;

# plot 'top view'
function qplot2p(x::Array{Float64}, y::Array{Float64}, name::ASCIIString, style::ASCIIString, w, time::Array{Float64})
	global qwtwPlot2H
	assert(length(x) == length(y))
	n = length(x)
	ww::Int32 = w;
	ccall(qwtwPlot2H, Void, (Ptr{Float64}, Ptr{Float64}, Int32, Ptr{Uint8}, Ptr{Uint8}, Int32, Int32, Ptr{Float64}),
		x, y, n, name, style, 1, ww, time);
	sleep(0.025)

end;

function qxlabel(s::ASCIIString)
	global qwtwXlabelH
	ccall(qwtwXlabelH, Void, (Ptr{Uint8},), s);
end;

function qylabel(s::ASCIIString)
	global qwtwYlabelH
	ccall(qwtwYlabelH, Void, (Ptr{Uint8},), s);
end;

function qtitle(s::String)
	global qwywTitleH
	ccall(qwywTitleH, Void, (Ptr{Uint8},), s);
end;

#=
function qtest(x::Array{Float64}, y::Array{Float64}, name::Ptr{Uint8}, style::Ptr{Uint8}, w)
	ww::Int32 = w;
	print(name)

end;
=#
export qfigure, qfmap, qsetmode, qplot, qplot1, qplot2, qplot2p, qxlabel,  qylabel, qtitle
export qimportant, qclear, qwtwStart, qwtwStop, qversion, qsmw
export traceit

end
