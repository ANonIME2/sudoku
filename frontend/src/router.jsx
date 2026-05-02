import { createBrowserRouter } from "react-router";
import Home from "./pages/home"
import Sandbox from "./pages/Sandbox"
import Layout from "./layout"
import StepsRenderer from "./components/StepsRenderer";

const router = createBrowserRouter([
  {
    path: "/",
    Component: Layout,
    children: [
      { index: true, Component: Home},
      { path:"/sandbox", Component:Sandbox},
      { path:"/steps", Component: StepsRenderer}
    ]
  }
]);

export default router;