from pymarchingCubes import MarchingSquares, compute_faster_wrapper
from math import cos, sin

import glfw
from OpenGL.GL import *
from OpenGL.GL.shaders import compileShader, compileProgram
import numpy as np


def func(x,y):
    return sin(5 * x) * cos(5 * y)/5
    # return sin(0.1 * x * x + 0.2 * y * y)
    # return (1-(x**2+y**3))*exp(-(x**2+y**2)/2)
    # return exp(sin(x) + cos(y)) -sin(exp(x+y))
    # return sin(sin(x) + cos(y)) - cos(sin(x*y) + cos(x))
    # return sin(x**2 + y**2) - cos(x*y)


vertex_src = """
# version 330 core

layout (location = 0)in vec3 a_position;

void main()
{
    gl_Position = vec4(a_position, 1.0);
}

"""

fragment_src = """
# version 330 core

out vec4 out_color;

void main()
{
    out_color = vec4(1.0, 0.65, 0, 1.0);
}

"""


def window_resize(window, width, height):
    glViewport(0, 0, width, height)



class Window():

    def __init__(self, title, resolution=(800, 600), position=(0, 0)):
        # Initialize glfw
        if not glfw.init():
            raise Exception("GLFW could not be initialized")

        self.title = title
        self.width , self.height = resolution
        self.x_pos, self.y_pos = position

        # Create the window
        self.window = glfw.create_window(self.width, self.height, title, None, None)

        # Check if the window was successfully create
        if not self.window:
            glfw.terminate()
            raise Exception("The window could not be created")

        glfw.set_window_pos(self.window, self.x_pos, self.y_pos)
        glfw.set_window_size_callback(self.window, window_resize)
        glfw.make_context_current(self.window)

        glClearColor(0, 0.1, 0.1, 0)

        # OpenGL goes from -1 to 1, always
        # TODO: Implement camera class to move around the scene  
        ms = MarchingSquares(func, [-1, 1], [-1,1], [250, 250])

        vertices, self.indices = compute_faster_wrapper(ms, 0.05)

        # Flatten out the array
        vertices = np.ravel(vertices)
        
        # Unfortunately, glDrawElements doesn't support uint64, so we made
        # incides to be unit32_t in C++ instead pf size_t (uinit64_t) 
        self.indices = np.ravel(self.indices)
        print("Done computing results")

        shader = compileProgram(compileShader(vertex_src, GL_VERTEX_SHADER),
                                compileShader(fragment_src, GL_FRAGMENT_SHADER))

        # Vertex buffer
        VBO = glGenBuffers(1)
        glBindBuffer(GL_ARRAY_BUFFER, VBO)
        glBufferData(GL_ARRAY_BUFFER, vertices.nbytes, vertices, GL_STATIC_DRAW)

        # Element buffer
        EBO = glGenBuffers(1)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO)
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, self.indices.nbytes, self.indices, GL_STATIC_DRAW)

        # i_position = glGetAttribLocation(shader, "a_position")
        glEnableVertexAttribArray(0)
        glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, 0, ctypes.c_void_p(0))

        glUseProgram(shader)
        glEnable(GL_DEPTH_TEST)


    def run(self):
        while not glfw.window_should_close(self.window):
            glfw.poll_events()
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

            # glDrawArrays(GL_TRIANGLE_STRIP, 0, 4)
            glDrawElements(GL_LINES, len(self.indices), GL_UNSIGNED_INT, ctypes.c_void_p(0))

            glfw.swap_buffers(self.window)

        glfw.terminate()


if __name__ == "__main__":
    window = Window("OpenGL", resolution=(800, 800), position=(200, 100))
    window.run()
