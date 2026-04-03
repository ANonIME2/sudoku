import { Outlet } from "react-router";
export default function Layout(){
  function render(){
    return (
      <div id="layout">
          <nav>
              <a href="/">play</a> |
              <a href="/sandbox">sandbox</a> 
          </nav>
          <main>
            <Outlet/>
          </main>
          <footer>Marcin Perzanowski 2026</footer>
      </div>
    )
  }
  return render();
}