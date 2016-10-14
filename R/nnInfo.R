#' nn_info - obtain basic information about an iQuest neural network model.
#'
#' @param model_name Name of the iQuest *.enn file that defines the neural network model.
#'
#' @return List containing number of input, output, and hidden neurons, respectively.
#' @export
#' @useDynLib nnlib nninfo
#'
#' @examples
#' filename <- file.path(system.file("extdata/WI_StreamTemp.enn", package="nnlib") )
#' nn_list <- nn_info( filename )  ## responds with a printout of key neural network features.
#' cat("There should be 17 input nodes. WI_StreamTemp.enn specifies", nn_list$num_input, "nodes.")
#' cat("There should be 1 output node. WI_StreamTemp.enn specifies", nn_list$num_output, "node.")
#' cat("There should be 3 hidden nodes. WI_StreamTemp.enn specifies", nn_list$num_hidden, "nodes.")
nn_info <- function( model_name ) {

model_num_input  <- integer(1)
model_num_output <- integer(1)
model_num_hidden <- integer(1)

num_input  <- integer(1)
num_output <- integer(1)
num_hidden <- integer(1)

# test to see whether the model file specified by user is readable
if( ! file.exists( model_name ) ) {
    stop("There is a problem opening your model file. Does it exist?\n",
       "  You specified file: ",dQuote( model_name ),"\n", call.=FALSE)
}

# make the actual call to the C function
num_input <- .C("nninfo",model_name,nin=as.integer(model_num_input),
                         as.integer(model_num_output),
                         as.integer(model_num_hidden) )$nin

num_output <- .C("nninfo",model_name,as.integer(model_num_input),
                          nout=as.integer(model_num_output),
                          as.integer(model_num_hidden) )$nout

num_hidden <- .C("nninfo",model_name,as.integer(model_num_input),
                               as.integer(model_num_output),
                               nhid=as.integer(model_num_hidden) )$nhid

return( list(num_input=num_input, num_output=num_output, num_hidden=num_hidden) )
}

